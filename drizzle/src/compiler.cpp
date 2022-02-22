#include "compiler.h"

#include <sh/ranges.h>
#include <sh/utility.h>

#include "dzbuiltin.h"
#include "dzstring.h"
#include "error.h"

Compiler::Compiler()
  : Compiler(Type::Main, nullptr) {}

Compiler::Compiler(Type type, Compiler* parent)
  : type(type), parent(parent), function(new DzFunction()) {}

auto Compiler::reservedStackSize() const -> std::size_t {
  return type == Type::Function ? 2 : 0;
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
  patchJumps(level.breaks);
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
  popVariables(std::distance(scope.data(), &level));
  level.breaks.push_back(emitJump(Opcode::Jump));
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
  level.continues.push_back(emitJump(Opcode::Jump));
}

void Compiler::visit(Statement::Def& def) {
  Compiler compiler(Type::Function, this);
  compiler.function->identifier = def.identifier;
  compiler.function->arity = def.arguments.size();
  compiler.locations.emplace(locations.top());
  compiler.increaseScope(Level::Type::Function);
  compiler.defineVariable(def.identifier);
  for (const auto& argument : def.arguments) {
    compiler.defineVariable(argument);
  }
  compiler.visit(def.statements);
  compiler.emitReturn();
  compiler.decreaseScope();
  compiler.locations.pop();

  auto function = compiler.function;
  for (auto c = this; c; c = c->parent) {
    function->definition += c->variables.size();
    function->definition += c->reservedStackSize();
  }

  emitConstant(function);
  defineVariable(def.identifier);
}

void Compiler::visit(Statement::ExpressionStatement& expression_statement) {
  AstVisiter::visit(expression_statement);
  emit(Opcode::Pop);
}

void Compiler::visit(Statement::If& if_) {
  std::vector<std::size_t> exits;

  for (auto& branch : if_.branches) {
    visit(branch.condition);
    const auto skip = emitJump(Opcode::JumpFalsePop);
    increaseScope(Level::Type::Branch);
    visit(branch.statements);
    decreaseScope();
    exits.push_back(emitJump(Opcode::Jump));
    patchJump(skip);
  }

  increaseScope(Level::Type::Branch);
  visit(if_.else_);
  decreaseScope();

  patchJumps(exits);
}

void Compiler::visit(Statement::Program& program) {
  increaseScope(Level::Type::Block);
  for (auto& builtin : DzBuiltIn::all()) {
    emitConstant(&builtin);
    defineVariable(Identifier(builtin.identifier, {}));
  }
  AstVisiter::visit(program);
  decreaseScope();
  emitReturn();
}

void Compiler::visit(Statement::Return& return_) {
  if (type == Type::Main) {
    throw SyntaxError(locations.top(), "no matching function");
  }
  AstVisiter::visit(return_);
  emit(Opcode::Return);
}

void Compiler::visit(Statement::Var& var) {
  AstVisiter::visit(var);
  defineVariable(var.identifier);
}

void Compiler::visit(Statement::While& while_) {
  const auto condition = function->chunk.size();
  visit(while_.condition);
  const auto exit = emitJump(Opcode::JumpFalsePop);

  increaseScope(Level::Type::Loop);
  visit(while_.statements);
  const auto level = decreaseScope();
  emitJump(Opcode::Jump, condition);

  patchJump(exit);
  patchJumps(level.breaks);
}

void Compiler::visit(Expr& expr) {
  locations.push(expr->location);
  AstVisiter::visit(expr);
  locations.pop();
}

void Compiler::visit(Expression::Assign& assign) {
  AstVisiter::visit(assign);
  store(assign.identifier);
}

void Compiler::visit(Expression::Binary& binary) {
  static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

  if (binary.type == Expression::Binary::Type::And) {
    visit(binary.left);
    const auto exit = emitJump(Opcode::JumpFalse);
    emit(Opcode::Pop);
    visit(binary.right);
    patchJump(exit);
    return;
  }

  if (binary.type == Expression::Binary::Type::Or) {
    visit(binary.left);
    const auto exit = emitJump(Opcode::JumpTrue);
    emit(Opcode::Pop);
    visit(binary.right);
    patchJump(exit);
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
  if (call.arguments.size() > std::numeric_limits<u8>::max()) {
    throw CompilerError(locations.top(), "too many arguments");
  }
  AstVisiter::visit(call);
  emit(Opcode::Call, call.arguments.size());
}

void Compiler::visit(Expression::Literal& literal) {
  static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);

  switch (literal.type) {
    case Expression::Literal::Type::Null:    emit(Opcode::Null); break;
    case Expression::Literal::Type::Boolean: emit(std::get<dzbool>(literal.value) ? Opcode::True : Opcode::False); break; 
    case Expression::Literal::Type::Float:   emitConstant(std::get<dzfloat>(literal.value)); break;
    case Expression::Literal::Type::Integer: emitConstant(std::get<dzint>(literal.value)); break;
    case Expression::Literal::Type::String:  emitConstant(new DzString(std::get<std::string>(literal.value))); break;
    default:
      SH_UNREACHABLE;
      break;
  }
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
  load(variable.identifier);
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
    throw CompilerError(locations.top(), "cannot encode '{}' in opcode '{}'", value, opcode);
  }
}

void Compiler::emitConstant(DzValue value) {
  emitExt(Opcode::Constant, function->chunk.constants.size());
  function->chunk.constants.push_back(value);
}

void Compiler::emitReturn() {
  emit(Opcode::Null, Opcode::Return);
}

auto Compiler::emitJump(Opcode opcode, std::size_t label) -> std::size_t {
  const auto jump = function->chunk.size();
  const auto offset = static_cast<s64>(label - jump) - kJumpBytes;
  if (offset < std::numeric_limits<s16>::min() || offset > -kJumpBytes) {
    throw CompilerError(locations.top(), "bad jump '{}'", offset);
  }
  emit(opcode, offset, offset >> 8);
  return jump;
}

void Compiler::patchJump(std::size_t jump) {
  const auto offset = static_cast<s64>(function->chunk.size() - jump) - kJumpBytes;
  if (offset < 0 || offset > std::numeric_limits<s16>::max()) {
    throw CompilerError(locations.top(), "bad jump '{}'", offset);
  }
  function->chunk.code[jump + 1] = offset;
  function->chunk.code[jump + 2] = offset >> 8;
}

void Compiler::patchJumps(const std::vector<std::size_t>& jumps) {
  for (const auto& jump : jumps) {
    patchJump(jump);
  }
}

void Compiler::load(const Identifier& identifier) {
  if (const auto index = resolve(identifier)) {
    emitExt(Opcode::Load, *index);
  } else if (const auto index = resolveAbsolute(identifier)) {
    emitExt(Opcode::LoadAbsolute, *index);
  } else {
    throw SyntaxError(identifier.location, "undefined variable '{}'", identifier);
  }
}

void Compiler::store(const Identifier& identifier) {
  if (const auto index = resolve(identifier)) {
    emitExt(Opcode::Store, *index);
  } else if (const auto index = resolveAbsolute(identifier)) {
    emitExt(Opcode::StoreAbsolute, *index);
  } else {
    throw SyntaxError(identifier.location, "undefined variable '{}'", identifier);
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

auto Compiler::resolveAbsolute(const Identifier& identifier) const -> std::optional<std::size_t> {
  for (auto compiler = parent; compiler; compiler = compiler->parent) {
    if (const auto index = compiler->resolve(identifier)) {
      if (compiler->parent) {
        throw SyntaxError(identifier.location, "captured variables must be global");
      }
      return index;
    }
  }
  return std::nullopt;
}

void Compiler::defineVariable(Identifier identifier) {
  for (const auto& variable : sh::reversed(variables)) {
    if (variable.depth != scope.size()) {
      break;
    } else if (variable.identifier == identifier) {
      throw SyntaxError(identifier.location, "redefined variable '{}'", identifier);
    }
  }
  variables.emplace_back(identifier, scope.size());
}

void Compiler::popVariables(std::size_t depth) {
  const auto size = variables.size();
  while (!variables.empty() && variables.back().depth > depth) {
    variables.pop_back();
  }

  const auto count = size - variables.size();
  if (count == 1) {
    emit(Opcode::Pop);
  } else if (count > 1) {
    emitExt(Opcode::PopMultiple, count);
  }
}

template<typename... Args>
  requires std::constructible_from<Compiler::Level, Args...>
void Compiler::increaseScope(Args&&... args) {
  scope.emplace_back(std::forward<Args>(args)...);
}

auto Compiler::decreaseScope() -> Compiler::Level {
  const auto level(std::move(scope.back()));
  scope.pop_back();
  patchJumps(level.continues);
  popVariables(scope.size());
  return level;
}
