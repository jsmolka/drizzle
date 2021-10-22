#pragma once

#include "expression.h"
#include "statement.h"

class Pass
{
public:
    void run(Stmt& ast);

protected:
    virtual void statementExpression(Stmt&) {}
    virtual void program(Stmt&) {}
    virtual void binary(Expr&) {}
    virtual void group(Expr&) {}
    virtual void literal(Expr&) {}
    virtual void unary(Expr&) {}

private:
    void walk(Stmt& stmt);
    void walk(Expr& expr);
};
