#include "astwalker.h"

#include <shell/macros.h>

void AstWalker::walk(Expr& expr)
{
    static_assert(int(Expression::Type::LastEnumValue) == 6);

    switch (expr->type)
    {
    case Expression::Type::Assign:   walk(expr->assign); break;
    case Expression::Type::Binary:   walk(expr->binary); break;
    case Expression::Type::Group:    walk(expr->group); break;
    case Expression::Type::Literal:  walk(expr->literal); break;
    case Expression::Type::Unary:    walk(expr->unary); break;
    case Expression::Type::Variable: walk(expr->variable); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void AstWalker::walk(Expression::Assign& assign)
{
    walk(assign.value);
}

void AstWalker::walk(Expression::Binary& binary)
{
    walk(binary.left);
    walk(binary.right);
}

void AstWalker::walk(Expression::Group& group)
{
    walk(group.expression);
}

void AstWalker::walk(Expression::Literal& literal)
{
}

void AstWalker::walk(Expression::Unary& unary)
{
    walk(unary.expression);
}

void AstWalker::walk(Expression::Variable& variable)
{
}

void AstWalker::walk(Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 7);

    switch (stmt->type)
    {
    case Statement::Type::Noop: break; 
    case Statement::Type::Block:               walk(stmt->block); break;
    case Statement::Type::ExpressionStatement: walk(stmt->expression_statement); break;
    case Statement::Type::If:                  walk(stmt->if_); break;
    case Statement::Type::Print:               walk(stmt->print); break;
    case Statement::Type::Program:             walk(stmt->program); break;
    case Statement::Type::Var:                 walk(stmt->var); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void AstWalker::walk(Stmts& stmts)
{
    for (auto& stmt : stmts)
        walk(stmt);
}

void AstWalker::walk(Statement::Block& block)
{
    walk(block.statements);
}

void AstWalker::walk(Statement::ExpressionStatement& expression_statement)
{
    walk(expression_statement.expression);
}

void AstWalker::walk(Statement::If& if_)
{
    walk(if_.if_.condition);
    walk(if_.if_.statements);

    for (auto& elif : if_.elifs)
    {
        walk(elif.condition);
        walk(elif.statements);
    }

    walk(if_.else_);
}

void AstWalker::walk(Statement::Print& print)
{
    walk(print.expression);
}

void AstWalker::walk(Statement::Program& program)
{
    walk(program.statements);
}

void AstWalker::walk(Statement::Var& var)
{
    walk(var.initializer);
}
