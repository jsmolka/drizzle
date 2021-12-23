#pragma once

#include "astwalker.h"
#include "chunk.h"
#include "opcode.h"
#include "stringpool.h"

class Compiler final : public AstWalker {
public:
  Compiler(StringPool& pool);

  void compile(Stmt& ast, Chunk& chunk);

protected:
  using AstWalker::walk;

  void walk(Stmt& stmt) final;
  void walk(Statement::Block& block) final;
  void walk(Statement::Break& break_) final;
  void walk(Statement::Continue& continue_) final;
  void walk(Statement::ExpressionStatement& expression_statement) final;
  void walk(Statement::If& if_) final;
  void walk(Statement::Print& print) final;
  void walk(Statement::Var& var) final;
  void walk(Statement::While& while_) final;

  void walk(Expr& expr) final;
  void walk(Expression::Assign& assign) final;
  void walk(Expression::Binary& binary) final;
  void walk(Expression::Literal& literal) final;
  void walk(Expression::Unary& unary) final;
  void walk(Expression::Variable& variable) final;

private:
  static constexpr auto kJumpBytes = 3;

  struct Level {
    enum class Type { Block, Branch, Loop, Function };

    Type type;
    std::string_view identifier;
    std::vector<std::size_t> breaks;
    std::vector<std::size_t> continues;
  };

  struct Variable {
    std::string_view identifier;
    std::size_t depth;
  };

  template<typename... Bytes>
  void emit(Bytes... bytes);
  void emitConstant(DzValue value);
  void emitVariable(Opcode opcode, std::size_t index);

  auto emitJump(Opcode opcode, std::size_t label = 0) -> std::size_t;
  void patchJump(std::size_t jump);
  void patchJumps(const std::vector<std::size_t>& jumps);

  void defineVariable(std::string_view identifier);
  auto resolveVariable(std::string_view identifier) -> Variable&;
  void popVariables(std::size_t depth);

  template<typename... Args>
    requires std::constructible_from<Compiler::Level, Args...>
  void increaseScope(Args&&... args);
  auto decreaseScope() -> Level;

  Chunk* chunk;
  std::size_t line;
  std::vector<Level> scope;
  std::vector<Variable> variables;
  StringPool& pool;
};
