#pragma once

#include "expression.h"
#include "statement.h"

class AstWalker
{
protected:
    virtual void walk(Expr& expr);
    virtual void walk(Expression::Assign& assign);
    virtual void walk(Expression::Binary& binary);
    virtual void walk(Expression::Group& group);
    virtual void walk(Expression::Literal& literal);
    virtual void walk(Expression::Unary& unary);
    virtual void walk(Expression::Variable& variable);

    virtual void walk(Stmt& stmt);
    virtual void walk(Stmts& stmts);
    virtual void walk(Statement::Block& block);
    virtual void walk(Statement::ExpressionStatement& expression_statement);
    virtual void walk(Statement::If& if_);
    virtual void walk(Statement::Noop& noop);
    virtual void walk(Statement::Print& print);
    virtual void walk(Statement::Program& program);
    virtual void walk(Statement::Var& var);
    virtual void walk(Statement::While& while_);
};
