#pragma once

#include <sh/vector.h>

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
  //void visit(Expression::Binary& binary) final;
  //void visit(Expression::Call& call) final;
  //void visit(Expression::Literal& literal) final;
  //void visit(Expression::Unary& unary) final;
  //void visit(Expression::Variable& variable) final;

private:
  struct Variable {
    enum class Type { Normal, Function };

    Type type;
    Identifier identifier;
  };

  Variable* resolve(const Identifier& identifier);

  sh::vector<Variable> variables;
};
