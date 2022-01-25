#pragma once

#include <sh/stack.h>

#include "astvisiter.h"
#include "dzfunction.h"
#include "opcode.h"
#include "stringpool.h"

class Compiler final : public AstVisiter {
public:
  Compiler(StringPool& pool);

  DzFunction* compile(const Stmt& ast);

protected:
  using AstVisiter::visit;

  void visit(Stmt& stmt) final;
  void visit(Statement::Block& block) final;
  void visit(Statement::Break& break_) final;
  void visit(Statement::Continue& continue_) final;
  void visit(Statement::Def& def) final;
  void visit(Statement::ExpressionStatement& expression_statement) final;
  void visit(Statement::If& if_) final;
  void visit(Statement::Print& print) final;
  void visit(Statement::Program& program) final;
  void visit(Statement::Var& var) final;
  void visit(Statement::While& while_) final;

  void visit(Expr& expr) final;
  void visit(Expression::Assign& assign) final;
  void visit(Expression::Binary& binary) final;
  void visit(Expression::Literal& literal) final;
  void visit(Expression::Unary& unary) final;
  void visit(Expression::Variable& variable) final;

private:
  static constexpr auto kJumpBytes = 3;

  enum class Type { Main, Function };

  struct Level {
    enum class Type { Block, Branch, Loop, Function };

    Type type;
    std::optional<std::string_view> identifier;
    std::vector<std::size_t> breaks;
    std::vector<std::size_t> continues;
  };

  struct Variable {
    std::string_view identifier;
    std::size_t depth;
  };

  Compiler(Type type, StringPool& pool, Compiler* parent);

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

  Type type;
  StringPool& pool;
  Compiler* parent;
  DzFunction* function;
  sh::stack<Location> locations;
  std::vector<Level> scope;
  std::vector<Variable> variables;
};
