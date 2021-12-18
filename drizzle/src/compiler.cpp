#include "compiler.h"

#include <sh/ranges.h>
#include <sh/utility.h>

#include "errors.h"

Compiler::Compiler(StringPool& pool) : pool(pool) {}

void Compiler::compile(Stmt& ast, Chunk& chunk) {
  this->chunk = &chunk;

  increaseScope(Level::Type::Block);
  walk(ast);
  decreaseScope();

  emit(Opcode::Return);
}

void Compiler::walk(Stmt& stmt) {
  line = stmt->location.line;

  AstWalker::walk(stmt);
}

void Compiler::walk(Statement::Block& block) {
  if (!block.identifier.empty()) {
    for (const auto& level : scope) {
      if (level.identifier == block.identifier)
        throw SyntaxError(level.identifier.data(), "redefined block '{}'", level.identifier);
    }
  }

  increaseScope(Level::Type::Block, block.identifier);
  AstWalker::walk(block);
  const auto level = decreaseScope();
  patchJumps(level.breaks);
}

void Compiler::walk(Statement::Break& break_) {
  const auto resolve = [this, &break_]() -> Level& {
    if (break_.identifier.empty()) {
      for (auto& level : sh::reversed(scope)) {
        if (level.type == Level::Type::Loop) return level;
      }
      throw SyntaxError(line, "no matching loop");
    } else {
      const auto identifier = break_.identifier;
      for (auto& level : sh::reversed(scope)) {
        if (level.identifier == identifier) return level;
      }
      throw SyntaxError(identifier.data(), "no matching block '{}'", identifier);
    }
  };

  auto& level = resolve();
  popVariables(std::distance(scope.data(), &level));
  level.breaks.push_back(emitJump(Opcode::Jump));
}

void Compiler::walk(Statement::Continue& continue_) {
  const auto resolve = [this]() -> Level& {
    for (auto& level : sh::reversed(scope)) {
      if (level.type == Level::Type::Loop) return level;
    }
    throw SyntaxError(line, "no matching loop");
  };
  resolve().continues.push_back(emitJump(Opcode::Jump));
}

void Compiler::walk(Statement::ExpressionStatement& expression_statement) {
  AstWalker::walk(expression_statement);
  emit(Opcode::Pop);
}

void Compiler::walk(Statement::If& if_) {
  std::vector<std::size_t> exits;

  const auto compile = [this, &exits](Statement::If::Branch& branch) {
    walk(branch.condition);
    const auto next = emitJump(Opcode::JumpFalsePop);
    increaseScope(Level::Type::Branch);
    walk(branch.statements);
    decreaseScope();
    exits.push_back(emitJump(Opcode::Jump));
    patchJump(next);
  };

  compile(if_.if_);
  for (auto& elif : if_.elifs)
    compile(elif);
  walk(if_.else_);

  patchJumps(exits);
}

void Compiler::walk(Statement::Print& print) {
  AstWalker::walk(print);
  emit(Opcode::Print);
}

void Compiler::walk(Statement::Var& var) {
  AstWalker::walk(var);
  defineVariable(var.identifier);
}

void Compiler::walk(Statement::While& while_) {
  const auto condition = chunk->label();
  walk(while_.condition);
  const auto exit = emitJump(Opcode::JumpFalsePop);

  increaseScope(Level::Type::Loop);
  walk(while_.statements);
  const auto level = decreaseScope();
  emitJump(Opcode::Jump, condition);

  patchJump(exit);
  patchJumps(level.breaks);
}

void Compiler::walk(Expr& expr) {
  line = expr->location.line;

  AstWalker::walk(expr);
}

void Compiler::walk(Expression::Assign& assign) {
  AstWalker::walk(assign);

  auto& var = resolveVariable(assign.identifier);
  emitVariable(std::distance(variables.data(), &var), Opcode::StoreVariable);
}

void Compiler::walk(Expression::Binary& binary) {
  static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

  if (binary.type == Expression::Binary::Type::And) {
    walk(binary.left);
    const auto exit = emitJump(Opcode::JumpFalse);
    emit(Opcode::Pop);
    walk(binary.right);
    patchJump(exit);
    return;
  }

  if (binary.type == Expression::Binary::Type::Or) {
    walk(binary.left);
    const auto exit = emitJump(Opcode::JumpTrue);
    emit(Opcode::Pop);
    walk(binary.right);
    patchJump(exit);
    return;
  }

  AstWalker::walk(binary);

  switch (binary.type) {
    case Expression::Binary::Type::Addition:
      emit(Opcode::Add);
      break;
    case Expression::Binary::Type::BitwiseAnd:
      emit(Opcode::BitwiseAnd);
      break;
    case Expression::Binary::Type::BitwiseAsr:
      emit(Opcode::BitwiseAsr);
      break;
    case Expression::Binary::Type::BitwiseLsl:
      emit(Opcode::BitwiseLsl);
      break;
    case Expression::Binary::Type::BitwiseLsr:
      emit(Opcode::BitwiseLsr);
      break;
    case Expression::Binary::Type::BitwiseOr:
      emit(Opcode::BitwiseOr);
      break;
    case Expression::Binary::Type::BitwiseXor:
      emit(Opcode::BitwiseXor);
      break;
    case Expression::Binary::Type::Division:
      emit(Opcode::Divide);
      break;
    case Expression::Binary::Type::Equal:
      emit(Opcode::Equal);
      break;
    case Expression::Binary::Type::Greater:
      emit(Opcode::Greater);
      break;
    case Expression::Binary::Type::GreaterEqual:
      emit(Opcode::GreaterEqual);
      break;
    case Expression::Binary::Type::IntegerDivision:
      emit(Opcode::DivideInt);
      break;
    case Expression::Binary::Type::Less:
      emit(Opcode::Less);
      break;
    case Expression::Binary::Type::LessEqual:
      emit(Opcode::LessEqual);
      break;
    case Expression::Binary::Type::Modulo:
      emit(Opcode::Modulo);
      break;
    case Expression::Binary::Type::Multiplication:
      emit(Opcode::Multiply);
      break;
    case Expression::Binary::Type::NotEqual:
      emit(Opcode::NotEqual);
      break;
    case Expression::Binary::Type::Power:
      emit(Opcode::Power);
      break;
    case Expression::Binary::Type::Subtraction:
      emit(Opcode::Subtract);
      break;

    default:
      SH_UNREACHABLE;
      break;
  }
}

void Compiler::walk(Expression::Literal& literal) {
  static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);

  switch (literal.type) {
    case Expression::Literal::Type::Null:
      emit(Opcode::Null);
      break;

    case Expression::Literal::Type::Boolean:
      emit(std::get<dzbool>(literal.value) ? Opcode::True : Opcode::False);
      break;

    case Expression::Literal::Type::Float:
      emitConstant(std::get<dzfloat>(literal.value));
      break;

    case Expression::Literal::Type::Integer:
      emitConstant(std::get<dzint>(literal.value));
      break;

    case Expression::Literal::Type::String:
      emitConstant(pool.make(std::string(std::get<std::string>(literal.value))));
      break;

    default:
      SH_UNREACHABLE;
      break;
  }
}

void Compiler::walk(Expression::Unary& unary) {
  static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);

  AstWalker::walk(unary);

  switch (unary.type) {
    case Expression::Unary::Type::BitwiseComplement:
      emit(Opcode::BitwiseComplement);
      break;
    case Expression::Unary::Type::Minus:
      emit(Opcode::Negate);
      break;
    case Expression::Unary::Type::Not:
      emit(Opcode::Not);
      break;

    default:
      SH_UNREACHABLE;
      break;
  }
}

void Compiler::walk(Expression::Variable& variable) {
  auto& var = resolveVariable(variable.identifier);
  emitVariable(std::distance(variables.data(), &var), Opcode::LoadVariable);
}

template <typename... Bytes>
void Compiler::emit(Bytes... bytes) {
  (chunk->write(static_cast<u8>(bytes), line), ...);
}

void Compiler::emitConstant(DzValue value) {
  const auto index = chunk->constants.size();
  if (index <= std::numeric_limits<u8>::max())
    emit(Opcode::Constant, index);
  else if (index <= std::numeric_limits<u16>::max())
    emit(Opcode::ConstantExt, index, index >> 8);
  else
    throw CompilerError(line, "constant limit exceeded");

  chunk->constants.push_back(value);
}

void Compiler::emitVariable(std::size_t index, Opcode opcode) {
  if (index <= std::numeric_limits<u8>::max())
    emit(opcode, index);
  else if (index <= std::numeric_limits<u16>::max())
    emit(Opcode(int(opcode) + 1), index, index >> 8);
  else
    throw CompilerError(line, "variable limit exceeded");
}

std::size_t Compiler::emitJump(Opcode opcode, std::size_t label) {
  const auto jump = chunk->label();

  s64 offset = static_cast<s64>(label - jump) - 3;
  if (offset < std::numeric_limits<s16>::min() || offset > -3)
    throw CompilerError(line, "bad jump '{}'", offset);

  emit(opcode, offset, offset >> 8);

  return jump;
}

void Compiler::patchJump(std::size_t jump) {
  s64 offset = static_cast<s64>(chunk->label() - jump) - 3;
  if (offset < 0 || offset > std::numeric_limits<s16>::max())
    throw CompilerError(line, "bad jump '{}'", offset);

  chunk->code[jump + 1] = offset;
  chunk->code[jump + 2] = offset >> 8;
}

void Compiler::patchJumps(const std::vector<std::size_t>& jumps) {
  for (const auto& jump : jumps)
    patchJump(jump);
}

void Compiler::defineVariable(std::string_view identifier) {
  for (const auto& variable : sh::reversed(variables)) {
    if (variable.depth != scope.size()) break;

    if (variable.identifier == identifier)
      throw SyntaxError(identifier.data(), "redefined '{}'", identifier);
  }
  variables.push_back({identifier, scope.size()});
}

Compiler::Variable& Compiler::resolveVariable(std::string_view identifier) {
  for (auto& variable : sh::reversed(variables)) {
    if (variable.identifier == identifier) return variable;
  }
  throw SyntaxError(identifier.data(), "undefined variable '{}'", identifier);
}

void Compiler::popVariables(std::size_t depth) {
  const auto size = variables.size();
  while (variables.size() && variables.back().depth > depth)
    variables.pop_back();

  const auto count = size - variables.size();
  if (count == 0)
    return;
  else if (count == 1)
    emit(Opcode::Pop);
  else if (count <= std::numeric_limits<u8>::max())
    emit(Opcode::PopMultiple, count);
  else if (count <= std::numeric_limits<u16>::max())
    emit(Opcode::PopMultipleExt, count, count >> 8);
  else
    throw CompilerError(line, "pop count too high '{}'", count);
}

template <typename... Args>
void Compiler::increaseScope(Args&&... args) {
  scope.push({std::forward<Args>(args)...});
}

Compiler::Level Compiler::decreaseScope() {
  const auto level = scope.pop_value();
  patchJumps(level.continues);
  popVariables(scope.size());

  return level;
}
