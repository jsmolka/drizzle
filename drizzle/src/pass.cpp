#include "pass.h"

#include <shell/macros.h>

void Pass::run(Stmt& ast)
{
    walk(ast);
}

void Pass::walk(Stmt& stmt)
{
    switch (stmt->kind)
    {
    case Statement::Kind::ExpressionStatement:
        walk(stmt->expression.expression);
        statementExpression(stmt);
        break;

    case Statement::Kind::Program:
        for (auto& expr : stmt->program.statements)
            walk(expr);
        program(stmt);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Pass::walk(Expr& expr)
{
    switch (expr->kind)
    {
    case Expression::Kind::Binary:
        walk(expr->binary.left);
        walk(expr->binary.right);
        binary(expr);
        break;

    case Expression::Kind::Group:
        walk(expr->group.expression);
        group(expr);
        break;

    case Expression::Kind::Literal:
        literal(expr);
        break;

    case Expression::Kind::Unary:
        walk(expr->unary.expression);
        unary(expr);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
