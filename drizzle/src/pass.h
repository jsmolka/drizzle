#pragma once

#include "expression.h"
#include "statement.h"

class Pass
{
public:
    void run(Stmt& ast);

protected:
    void walk(Stmt& stmt);
    void walk(Expr& expr);

    virtual void before(Stmt&) {}
    virtual void after(Stmt&) {}
    virtual void before(Expr&) {}
    virtual void after(Expr&) {}
};
