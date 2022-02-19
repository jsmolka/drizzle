#pragma once

#include <vector>

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

  void visit(Expression::Assign& assign) final;

private:
  struct Variable {
    enum class Type { Normal, Function };

    std::size_t depth;
    Identifier identifier;
    Type type;
  };

  void increaseScope();
  void decreaseScope();

  template<typename... Args>
  void define(Args&&... args);
  Variable* resolve(const Identifier& identifier);
  void assign(const Identifier& identifier, const Expr& value);

  std::size_t depth = 0;
  std::vector<Variable> variables;
};
