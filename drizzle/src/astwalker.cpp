#include "astwalker.h"

#include <shell/macros.h>

void AstWalker::walk(Stmt& stmt)
{
    before(stmt);

    switch (stmt->type)
    {
    case Statement::Type::Block:
        for (auto& stmt : stmt->block.statements)
            walk(stmt);
        break;

    case Statement::Type::ExpressionStatement:
        walk(stmt->expression_statement.expression);
        break;

    case Statement::Type::Program:
        for (auto& stmt : stmt->program.statements)
            walk(stmt);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    after(stmt);
}

void AstWalker::walk(Expr& expr)
{
    before(expr);

    switch (expr->type)
    {
    case Expression::Type::Binary:
        walk(expr->binary.left);
        walk(expr->binary.right);
        break;

    case Expression::Type::Group:
        walk(expr->group.expression);
        break;

    case Expression::Type::Literal:
        break;

    case Expression::Type::Unary:
        walk(expr->unary.expression);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    after(expr);
}
