#include "astwalker.h"

#include <shell/macros.h>

void AstWalker::walk(Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 6);

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

    case Statement::Type::Noop:
        break;

    case Statement::Type::Print:
        walk(stmt->print.expression);
        break;

    case Statement::Type::Program:
        for (auto& stmt : stmt->program.statements)
            walk(stmt);
        break;

    case Statement::Type::Var:
        walk(stmt->var.initializer);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    after(stmt);
}

void AstWalker::walk(Expr& expr)
{
    static_assert(int(Expression::Type::LastEnumValue) == 6);

    before(expr);

    switch (expr->type)
    {
    case Expression::Type::Assign:
        walk(expr->assign.value);
        break;

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

    case Expression::Type::Variable:
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    after(expr);
}
