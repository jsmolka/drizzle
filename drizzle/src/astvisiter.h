#pragma once

#include "expression.h"
#include "statement.h"

class AstVisiter {
protected:
  virtual void visit(Expr& expr);
  virtual void visit(Exprs& exprs);
  virtual void visit(Expression::Assign& assign);
  virtual void visit(Expression::Binary& binary);
  virtual void visit(Expression::Call& call);
  virtual void visit(Expression::Get& get);
  virtual void visit(Expression::Group& group);
  virtual void visit(Expression::Literal& literal);
  virtual void visit(Expression::Set& set);
  virtual void visit(Expression::Unary& unary);
  virtual void visit(Expression::Variable& variable);

  virtual void visit(Stmt& stmt);
  virtual void visit(Stmts& stmts);
  virtual void visit(Statement::Block& block);
  virtual void visit(Statement::Break& break_);
  virtual void visit(Statement::Class& class_);
  virtual void visit(Statement::Continue& continue_);
  virtual void visit(Statement::Def& def);
  virtual void visit(Statement::ExpressionStatement& expression_statement);
  virtual void visit(Statement::If& if_);
  virtual void visit(Statement::Noop& noop);
  virtual void visit(Statement::Program& program);
  virtual void visit(Statement::Return& return_);
  virtual void visit(Statement::Var& var);
  virtual void visit(Statement::While& while_);
};
