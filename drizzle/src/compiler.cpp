#include "compiler.h"

#include <sh/ranges.h>
#include <sh/utility.h>

#include "dzclass.h"
#include "error.h"
#include "gc.h"

Compiler::Compiler(Gc& gc)
  : Compiler(gc, Type::Main, gc.construct<DzFunction>(), nullptr) {}

Compiler::Compiler(Gc& gc, Type type, DzFunction* function, Compiler* parent)
  : gc(gc), type(type), function(function), parent(parent) {}

auto Compiler::compile(const Stmt& ast) -> DzFunction* {
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
      if (level.type == Level::Type::Block && level.identifier == *block.identifier) {
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
      throw SyntaxError(identifier->location, "unknown block '{}'", *identifier);
    } else {
      for (auto& level : sh::reversed(scope)) {
        if (level.type == Level::Type::Loop) {
          return level;
        }
      }
      throw SyntaxError(locations.top(), "'break' outside loop");
    }
  };

  auto& level = resolve(break_.identifier);
  pop(std::distance(scope.data(), &level));
  level.breaks.push_back(jump(Opcode::Jump));
}

void Compiler::visit(Statement::Class& class_) {
  auto object = gc.construct<DzClass>(gc.construct<DzString>(class_.identifier));
  for (const auto& method : class_.methods) {
    auto& def = method->def;

    const auto type = def.identifier == DzClass::kInit ? Type::Init : Type::Function;
    const auto identifier = gc.construct<DzString>(def.identifier);
    const auto function = gc.construct<DzFunction>(identifier, Arity::equal(def.parameters.size()));
    Compiler compiler(gc, type, function, this);

    compiler.define("this");
    for (const auto& parameter : def.parameters) {
      compiler.define(parameter);
    }

    compiler.increaseScope(Level::Type::Function);
    compiler.visit(def.statements);
    compiler.decreaseScope();
    compiler.emit(Opcode::Load, 0, Opcode::Return);

    object->set(function->identifier, function);
  }

  emitConstant(object);
  define(class_.identifier);
}

void Compiler::visit(Statement::Continue& continue_) {
  auto resolve = [this]() -> Level& {
    for (auto& level : sh::reversed(scope)) {
      if (level.type == Level::Type::Loop) {
        return level;
      }
    }
    throw SyntaxError(locations.top(), "'continue' outside loop");
  };

  auto& level = resolve();
  level.continues.push_back(jump(Opcode::Jump));
}

void Compiler::visit(Statement::Def& def) {
  const auto identifier = gc.construct<DzString>(def.identifier);
  const auto function = gc.construct<DzFunction>(identifier, Arity::equal(def.parameters.size()));
  Compiler compiler(gc, Type::Function, function, this);

  compiler.define(def.identifier);
  for (const auto& parameter : def.parameters) {
    compiler.define(parameter);
  }

  compiler.increaseScope(Level::Type::Function);
  compiler.visit(def.statements);
  compiler.decreaseScope();
  compiler.emit(Opcode::Null, Opcode::Return);

  emitConstant(compiler.function);
  define(def.identifier);
}

void Compiler::visit(Statement::ExpressionStatement& expression_statement) {
  AstVisiter::visit(expression_statement);
  emit(Opcode::Pop);
}

void Compiler::visit(Statement::For& for_) {
  increaseScope(Level::Type::Block);

  std::size_t condition;
  std::size_t exit;
  if (for_.iteree->type == Expression::Type::Range) {
    visit(for_.iteree->range.start);
    const auto iter = defineLocal("$iter");
    visit(for_.iteree->range.stop);
    const auto stop = defineLocal("$stop");

    condition = function->chunk().size();
    emitExt(Opcode::Load, iter);
    emitExt(Opcode::Load, stop);
    emit(Opcode::Less);
    exit = jump(Opcode::JumpFalsePop);

    increaseScope(Level::Type::Loop);
    emitExt(Opcode::Load, iter);
    define(for_.iterator);
    emitExt(Opcode::Load, iter);
    emitConstant(static_cast<dzint>(1));
    emit(Opcode::Add);
    emitExt(Opcode::Store, iter);
    emit(Opcode::Pop);
  } else {
    visit(for_.iteree);
    emit(Opcode::IterInit);
    const auto iter = defineLocal("$iter");

    condition = function->chunk().size();
    emitExt(Opcode::Load, iter);
    exit = jump(Opcode::JumpFalsePop);

    increaseScope(Level::Type::Loop);
    emitExt(Opcode::IterCurrent, iter);
    define(for_.iterator);
    emitExt(Opcode::IterAdvance, iter);
  }

  visit(for_.statements);
  const auto level = decreaseScope();
  jump(Opcode::Jump, condition);

  patch(exit);
  patch(level.breaks);

  decreaseScope();
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
  AstVisiter::visit(program);
  emit(Opcode::Exit);

  function->identifiers.reserve(globals.size());
  for (const auto& [identifier, global] : globals) {
    function->identifiers.insert({ gc.construct<DzString>(identifier), global.index });
  }
}

void Compiler::visit(Statement::Return& return_) {
  if (type == Type::Main) {
    throw SyntaxError(locations.top(), "'return' outside function");
  } else if (type == Type::Init) {
    if (return_.expression) {
      throw SyntaxError(locations.top(), "init() cannot return value");
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
  const auto condition = function->chunk().size();
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
  } else {
    emitExt(Opcode::StoreGlobal, resolveGlobal(identifier).index);
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

  visit(binary.left);
  visit(binary.right);
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
    throw CompilerError(locations.top(), "cannot encode argument count '{}'", arguments);
  }
  visit(call.callee);
  visit(call.arguments);
  emit(Opcode::Call, arguments);
}

void Compiler::visit(Expression::Get& get) {
  visit(get.self);
  emitConstant(gc.construct<DzString>(get.identifier));
  emit(Opcode::Get);
}

void Compiler::visit(Expression::In& in) {
  visit(in.self);
  visit(in.expression);
  emit(Opcode::In);
}

void Compiler::visit(Expression::Invoke& invoke) {
  const auto arguments = invoke.arguments.size();
  if (arguments > std::numeric_limits<u8>::max()) {
    throw CompilerError(locations.top(), "cannot encode argument count '{}'", arguments);
  }
  visit(invoke.self);
  visit(invoke.arguments);
  emitConstant(gc.construct<DzString>(invoke.identifier));
  emit(Opcode::Invoke, arguments);
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

void Compiler::visit(Expression::List& list) {
  AstVisiter::visit(list);
  emitExt(Opcode::List, list.values.size());
}

void Compiler::visit(Expression::Map& map) {
  for (auto& [key, value] : sh::reversed(map.pairs)) {
    visit(key);
    visit(value);
  }
  emitExt(Opcode::Map, map.pairs.size());
}

void Compiler::visit(Expression::Range& range) {
  visit(range.start);
  visit(range.stop);
  emit(Opcode::Range);
}

void Compiler::visit(Expression::Set& set) {
  visit(set.value);
  visit(set.self);
  emitConstant(gc.construct<DzString>(set.identifier));
  emit(Opcode::Set);
}

void Compiler::visit(Expression::SubscriptGet& subscript_get) {
  visit(subscript_get.self);
  visit(subscript_get.expression);
  emit(Opcode::SubscriptGet);
}

void Compiler::visit(Expression::SubscriptSet& subscript_set) {
  visit(subscript_set.value);
  visit(subscript_set.self);
  visit(subscript_set.expression);
  emit(Opcode::SubscriptSet);
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
  } else {
    emitExt(Opcode::LoadGlobal, resolveGlobal(identifier).index);
  }
}

template<typename... Bytes>
void Compiler::emit(Bytes... bytes) {
  const auto line = locations.empty() ? 0 : locations.top().line;
  (function->chunk().write(static_cast<u8>(bytes), line), ...);
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

void Compiler::emitConstant(const DzValue& value) {
  auto& chunk = function->chunk();
  emitExt(Opcode::Constant, chunk.constants.size());
  chunk.constants.push_back(value);
}

auto Compiler::jump(Opcode opcode, std::optional<std::size_t> label) -> std::size_t {
  const auto jump = function->chunk().size();
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
  auto& chunk = function->chunk();
  const auto offset = static_cast<s64>(chunk.size() - jump - kJumpSize);
  if (offset < 0 || offset > std::numeric_limits<s16>::max()) {
    throw CompilerError(locations.top(), "cannot encode jump offset '{}'", offset);
  }
  chunk.code[jump + 1] = offset;
  chunk.code[jump + 2] = offset >> 8;
}

void Compiler::patch(const std::vector<std::size_t>& jumps) {
  for (const auto& jump : jumps) {
    patch(jump);
  }
}

auto Compiler::defineLocal(const Identifier& identifier) -> std::size_t {
  for (const auto& variable : sh::reversed(variables)) {
    if (variable.depth != scope.size()) {
      break;
    } else if (variable.identifier == identifier) {
      throw SyntaxError(identifier.location, "redefined variable '{}'", identifier);
    }
  }
  variables.emplace(identifier, scope.size());
  return variables.size() - 1;
}

void Compiler::define(const Identifier& identifier) {
  auto redefined = [](const Identifier& identifier) {
    throw SyntaxError(identifier.location, "redefined variable '{}'", identifier);
  };

  if (parent || !scope.empty()) {
    defineLocal(identifier);
  } else {
    auto& global = resolveGlobal(identifier);
    if (global.defined) {
      throw SyntaxError(identifier.location, "redefined variable '{}'", identifier);
    }
    global.defined = true;
    emitExt(Opcode::StoreGlobal, global.index);
    emit(Opcode::Pop);
  }
}

auto Compiler::resolve(const Identifier& identifier) const -> std::optional<std::size_t> {
  for (const auto& variable : sh::reversed(variables)) {
    if (variable.identifier == identifier) {
      return std::distance(variables.data(), &variable);
    }
  }
  return std::nullopt;
}

auto Compiler::resolveGlobal(const Identifier& identifier) -> Global& {
  auto compiler = this;
  while (compiler->parent) {
    compiler = compiler->parent;
    if (const auto index = compiler->resolve(identifier)) {
      if (compiler->parent) {
        throw SyntaxError(identifier.location, "cannot capture local variable");
      }
    }
  }
  const auto [iter, inserted] = compiler->globals.try_emplace(identifier, compiler->globals.size(), false);
  return iter.value();
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
