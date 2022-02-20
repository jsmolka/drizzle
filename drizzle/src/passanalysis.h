#pragma once

#include <sh/stack.h>

#include "astvisiter.h"

class PassAnalysis : public AstVisiter {
public:
  void run(const Stmt& ast);

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
  enum class Type { None, Function, Closure };

  struct Variable {
    std::size_t depth;
    Identifier identifier;
    Type type = Type::None;
  };

  void increase();
  void decrease();

  template<typename... Args>
  void define(Args&&... args);
  void resolve(const Identifier& identifier);

  bool callee = false;
  std::size_t depth = 0;
  sh::stack<Type> functions;
  sh::stack<Variable> variables;
};
