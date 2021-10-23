#include "pass.h"

#include <shell/macros.h>

void Pass::run(Stmt& ast)
{
    walk(ast);
}

void Pass::walk(Stmt& stmt)
{
    before(stmt);

    switch (stmt->kind)
    {
    case Statement::Kind::ExpressionStatement:
        walk(stmt->expression.expression);
        break;

    case Statement::Kind::Program:
        for (auto& stmt : stmt->program.statements)
            walk(stmt);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    after(stmt);
}

void Pass::walk(Expr& expr)
{
    before(expr);

    switch (expr->kind)
    {
    case Expression::Kind::Binary:
        walk(expr->binary.left);
        walk(expr->binary.right);
        break;

    case Expression::Kind::Group:
        walk(expr->group.expression);
        break;

    case Expression::Kind::Literal:
        break;

    case Expression::Kind::Unary:
        walk(expr->unary.expression);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    after(expr);
}
