#pragma once

#include <sh/stack.h>

#include "astvisiter.h"

class PassAnalysis : public AstVisiter {
public:
  void run(Stmt& ast);

protected:
  using AstVisiter::visit;

  void visit(Statement::Block& block) final;
  void visit(Statement::Def& def) final;
  void visit(Statement::If& if_) final;
  void visit(Statement::Var& var) final;
  void visit(Statement::While& while_) final;

  void visit(Expression::Call& call) final;
  void visit(Expression::Variable& variable) final;

private:
  enum class Level { Block, Branch, Loop, Function, Closure };

  struct Variable {
    enum class Type { Unknown, Function, Closure };

    std::size_t depth;
    Identifier identifier;
    Type type = Type::Unknown;
  };

  void increase(Level level);
  auto decrease() -> Level;

  template<typename... Args>
  void define(Args&&... args);
  void resolve(const Identifier& identifier);

  bool callee = false;
  sh::stack<Level> scope;
  sh::stack<Variable> variables;
};
