#include "compiler.h"

#include <sh/ranges.h>
#include <sh/utility.h>

#include "dzbuiltin.h"
#include "dzclass.h"
#include "dzstring.h"
#include "error.h"
#include "gc.h"

Compiler::Compiler(Gc& gc)
  : Compiler(gc, Type::Main, nullptr) {}

Compiler::Compiler(Gc& gc, Type type, Compiler* parent)
  : gc(gc), type(type), parent(parent), function(gc.construct<DzFunction>()) {
  if (parent) {
    locations.push(parent->locations.top());
  }
}

DzFunction* Compiler::compile(const Stmt& ast) {
  visit(const_cast<Stmt&>(ast));
  return function;
}

void Compiler::visit(Stmt& stmt) {
  locations.push(stmt->location);
  AstVisiter::visit(stmt);
  locations.pop();
}

void Compiler::visit(Statement::Block& block) {
  if (block.identifier) {
    for (const auto& level : scope) {
      if (level.identifier == *block.identifier) {
        throw SyntaxError(block.identifier->location, "redefined block '{}'", *block.identifier);
      }
    }
  }

  increaseScope(Level::Type::Block, block.identifier);
  AstVisiter::visit(block);
  const auto level = decreaseScope();
  patch(level.breaks);
}

void Compiler::visit(Statement::Break& break_) {
  auto resolve = [this](std::optional<Identifier> identifier) -> Level& {
    if (identifier) {
      for (auto& level : sh::reversed(scope)) {
        if (level.identifier == *identifier) {
          return level;
        }
      }
      throw SyntaxError(identifier->location, "no matching block '{}'", *identifier);
    } else {
      for (auto& level : sh::reversed(scope)) {
        if (level.type == Level::Type::Loop) {
          return level;
        }
      }
      throw SyntaxError(locations.top(), "no matching loop");
    }
  };

  auto& level = resolve(break_.identifier);
  pop(std::distance(scope.data(), &level));
  level.breaks.push_back(jump(Opcode::Jump));
}

void Compiler::visit(Statement::Class& class_) {
  define(class_.identifier);

  auto object = gc.construct<DzClass>(gc.construct<DzString>(class_.identifier));
  for (const auto& method : class_.methods) {
    auto& def = method->def;

    Compiler compiler(gc, def.identifier == DzClass::kInit ? Type::Init : Type::Function, this);
    compiler.function->identifier = gc.construct<DzString>(def.identifier);
    compiler.function->arity = def.parameters.size();

    compiler.define("this");
    for (const auto& parameter : def.parameters) {
      compiler.define(parameter);
    }

    compiler.increaseScope(Level::Type::Function);
    compiler.visit(def.statements);
    compiler.decreaseScope();
    compiler.emit(Opcode::Load, 0, Opcode::Return);

    object->methods.push_back(compiler.function);
  }
  emitConstant(object);
}

void Compiler::visit(Statement::Continue& continue_) {
  auto resolve = [this]() -> Level& {
    for (auto& level : sh::reversed(scope)) {
      if (level.type == Level::Type::Loop) {
        return level;
      }
    }
    throw SyntaxError(locations.top(), "no matching loop");
  };

  auto& level = resolve();
  level.continues.push_back(jump(Opcode::Jump));
}

void Compiler::visit(Statement::Def& def) {
  define(def.identifier);

  Compiler compiler(gc, Type::Function, this);
  compiler.function->identifier = gc.construct<DzString>(def.identifier);
  compiler.function->arity = def.parameters.size();

  compiler.define(def.identifier);
  for (const auto& parameter : def.parameters) {
    compiler.define(parameter);
  }

  compiler.increaseScope(Level::Type::Function);
  compiler.visit(def.statements);
  compiler.decreaseScope();
  compiler.emit(Opcode::Null, Opcode::Return);

  emitConstant(compiler.function);
}

void Compiler::visit(Statement::ExpressionStatement& expression_statement) {
  AstVisiter::visit(expression_statement);
  emit(Opcode::Pop);
}

void Compiler::visit(Statement::If& if_) {
  std::vector<std::size_t> exits;
  for (auto& branch : if_.branches) {
    visit(branch.condition);
    const auto next = jump(Opcode::JumpFalsePop);
    increaseScope(Level::Type::Branch);
    visit(branch.statements);
    decreaseScope();
    exits.push_back(jump(Opcode::Jump));
    patch(next);
  }

  increaseScope(Level::Type::Branch);
  visit(if_.else_);
  decreaseScope();

  patch(exits);
}

void Compiler::visit(Statement::Program& program) {
  increaseScope(Level::Type::Block);
  for (auto& builtin : DzBuiltIn::all) {
    emitConstant(&builtin);
    define(builtin.identifier);
  }
  AstVisiter::visit(program);
  decreaseScope();
  emit(Opcode::Exit);
}

void Compiler::visit(Statement::Return& return_) {
  if (type == Type::Main) {
    throw SyntaxError(locations.top(), "no matching function");
  } else if (type == Type::Init) {
    if (return_.expression) {
      throw SyntaxError(locations.top(), "cannot return value from initializer");
    }
    emit(Opcode::Load, 0, Opcode::Return);
  } else {
    AstVisiter::visit(return_);
    emit(Opcode::Return);
  }
}

void Compiler::visit(Statement::Var& var) {
  AstVisiter::visit(var);
  define(var.identifier);
}

void Compiler::visit(Statement::While& while_) {
  const auto condition = function->chunk.size();
  visit(while_.condition);
  const auto exit = jump(Opcode::JumpFalsePop);

  increaseScope(Level::Type::Loop);
  visit(while_.statements);
  const auto level = decreaseScope();
  jump(Opcode::Jump, condition);

  patch(exit);
  patch(level.breaks);
}

void Compiler::visit(Expr& expr) {
  locations.push(expr->location);
  AstVisiter::visit(expr);
  locations.pop();
}

void Compiler::visit(Expression::Assign& assign) {
  AstVisiter::visit(assign);
  const auto& identifier = assign.identifier;
  if (const auto index = resolve(identifier)) {
    emitExt(Opcode::Store, *index);
  } else if (const auto index = resolveAbsolute(identifier)) {
    emitExt(Opcode::StoreAbsolute, *index);
  } else {
    throw SyntaxError(identifier.location, "undefined variable '{}'", identifier);
  }
}

void Compiler::visit(Expression::Binary& binary) {
  static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

  if (binary.type == Expression::Binary::Type::And) {
    visit(binary.left);
    const auto skip = jump(Opcode::JumpFalse);
    emit(Opcode::Pop);
    visit(binary.right);
    patch(skip);
    return;
  }

  if (binary.type == Expression::Binary::Type::Or) {
    visit(binary.left);
    const auto skip = jump(Opcode::JumpTrue);
    emit(Opcode::Pop);
    visit(binary.right);
    patch(skip);
    return;
  }

  AstVisiter::visit(binary);
  switch (binary.type) {
    case Expression::Binary::Type::Addition:        emit(Opcode::Add); break;
    case Expression::Binary::Type::BitwiseAnd:      emit(Opcode::BitwiseAnd); break;
    case Expression::Binary::Type::BitwiseAsr:      emit(Opcode::BitwiseAsr); break;
    case Expression::Binary::Type::BitwiseLsl:      emit(Opcode::BitwiseLsl); break;
    case Expression::Binary::Type::BitwiseLsr:      emit(Opcode::BitwiseLsr); break;
    case Expression::Binary::Type::BitwiseOr:       emit(Opcode::BitwiseOr); break;
    case Expression::Binary::Type::BitwiseXor:      emit(Opcode::BitwiseXor); break;
    case Expression::Binary::Type::Division:        emit(Opcode::Divide); break;
    case Expression::Binary::Type::Equal:           emit(Opcode::Equal); break;
    case Expression::Binary::Type::Greater:         emit(Opcode::Greater); break;
    case Expression::Binary::Type::GreaterEqual:    emit(Opcode::GreaterEqual); break;
    case Expression::Binary::Type::IntegerDivision: emit(Opcode::DivideInt); break;
    case Expression::Binary::Type::Less:            emit(Opcode::Less); break;
    case Expression::Binary::Type::LessEqual:       emit(Opcode::LessEqual); break;
    case Expression::Binary::Type::Modulo:          emit(Opcode::Modulo); break;
    case Expression::Binary::Type::Multiplication:  emit(Opcode::Multiply); break;
    case Expression::Binary::Type::NotEqual:        emit(Opcode::NotEqual); break;
    case Expression::Binary::Type::Power:           emit(Opcode::Power); break;
    case Expression::Binary::Type::Subtraction:     emit(Opcode::Subtract); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void Compiler::visit(Expression::Call& call) {
  const auto arguments = call.arguments.size();
  if (arguments > std::numeric_limits<u8>::max()) {
    throw CompilerError(locations.top(), "cannot encode call argument count '{}'", arguments);
  }
  AstVisiter::visit(call);
  emit(Opcode::Call, arguments);
}

void Compiler::visit(Expression::Get& get) {
  visit(get.self);
  emitConstant(gc.construct<DzString>(get.identifier));
  emit(Opcode::Get);
}

void Compiler::visit(Expression::Literal& literal) {
  static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);

  switch (literal.type) {
    case Expression::Literal::Type::Null:    emit(Opcode::Null); break;
    case Expression::Literal::Type::Boolean: emit(std::get<dzbool>(literal.value) ? Opcode::True : Opcode::False); break; 
    case Expression::Literal::Type::Integer: emitConstant(std::get<dzint>(literal.value)); break;
    case Expression::Literal::Type::Float:   emitConstant(std::get<dzfloat>(literal.value)); break;
    case Expression::Literal::Type::String:  emitConstant(gc.construct<DzString>(std::get<std::string>(literal.value))); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void Compiler::visit(Expression::Set& set) {
  visit(set.value);
  visit(set.self);
  emitConstant(gc.construct<DzString>(set.identifier));
  emit(Opcode::Set);
}

void Compiler::visit(Expression::Unary& unary) {
  static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);

  AstVisiter::visit(unary);
  switch (unary.type) {
    case Expression::Unary::Type::BitwiseComplement: emit(Opcode::BitwiseComplement); break;
    case Expression::Unary::Type::Minus:             emit(Opcode::Negate); break;
    case Expression::Unary::Type::Not:               emit(Opcode::Not); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void Compiler::visit(Expression::Variable& variable) {
  const auto& identifier = variable.identifier;
  if (const auto index = resolve(identifier)) {
    emitExt(Opcode::Load, *index);
  } else if (const auto index = resolveAbsolute(identifier)) {
    emitExt(Opcode::LoadAbsolute, *index);
  } else {
    throw SyntaxError(identifier.location, "undefined variable '{}'", identifier);
  }
}

template<typename... Bytes>
void Compiler::emit(Bytes... bytes) {
  (function->chunk.write(static_cast<u8>(bytes), locations.top().line), ...);
}

void Compiler::emitExt(Opcode opcode, std::size_t value) {
  if (value <= std::numeric_limits<u8>::max()) {
    emit(opcode, value);
  } else if (value <= std::numeric_limits<u16>::max()) {
    emit(Opcode(int(opcode) + 1), value, value >> 8);
  } else {
    throw CompilerError(locations.top(), "cannot encode opcode '{}' value '{}'", opcode, value);
  }
}

void Compiler::emitConstant(DzValue value) {
  emitExt(Opcode::Constant, function->chunk.constants.size());
  function->chunk.constants.push_back(value);
}

auto Compiler::jump(Opcode opcode, std::optional<std::size_t> label) -> std::size_t {
  const auto jump = function->chunk.size();
  if (label) {
    const auto offset = static_cast<s64>(*label - jump - kJumpSize);
    if (offset < std::numeric_limits<s16>::min() || offset > -kJumpSize) {
      throw CompilerError(locations.top(), "cannot encode jump offset '{}'", offset);
    }
    emit(opcode, offset, offset >> 8);
  } else {
    emit(opcode, 0, 0);
  }
  return jump;
}

void Compiler::patch(std::size_t jump) {
  const auto offset = static_cast<s64>(function->chunk.size() - jump - kJumpSize);
  if (offset < 0 || offset > std::numeric_limits<s16>::max()) {
    throw CompilerError(locations.top(), "cannot encode jump offset '{}'", offset);
  }
  function->chunk.code[jump + 1] = offset;
  function->chunk.code[jump + 2] = offset >> 8;
}

void Compiler::patch(const std::vector<std::size_t>& jumps) {
  for (const auto& jump : jumps) {
    patch(jump);
  }
}

void Compiler::define(const Identifier& identifier) {
  for (const auto& variable : sh::reversed(variables)) {
    if (variable.depth != scope.size()) {
      break;
    } else if (variable.identifier == identifier) {
      throw SyntaxError(identifier.location, "redefined variable '{}'", identifier);
    }
  }
  variables.emplace(identifier, scope.size());
}

auto Compiler::resolve(const Identifier& identifier) const -> std::optional<std::size_t> {
  for (const auto& variable : sh::reversed(variables)) {
    if (variable.identifier == identifier) {
      return std::distance(variables.data(), &variable);
    }
  }
  return std::nullopt;
}

auto Compiler::resolveAbsolute(const Identifier& identifier) const -> std::optional<std::size_t> {
  for (auto compiler = parent; compiler; compiler = compiler->parent) {
    if (const auto index = compiler->resolve(identifier)) {
      if (compiler->parent) {
        throw SyntaxError(identifier.location, "cannot capture local variable");
      }
      return index;
    }
  }
  return std::nullopt;
}

void Compiler::pop(std::size_t depth) {
  const auto size = variables.size();
  while (!variables.empty() && variables.top().depth > depth) {
    variables.pop();
  }

  const auto count = size - variables.size();
  if (count == 1) {
    emit(Opcode::Pop);
  } else if (count > 1) {
    emitExt(Opcode::PopMultiple, count);
  }
}

template<typename... Args>
void Compiler::increaseScope(Args&&... args) {
  scope.emplace(std::forward<Args>(args)...);
}

auto Compiler::decreaseScope() -> Level {
  auto level = scope.pop_value();
  patch(level.continues);
  pop(scope.size());
  return level;
}
