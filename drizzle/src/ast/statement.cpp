#include "statement.h"

#include <shell/macros.h>

#include "expression.h"

Statement::ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression)
    : expression(std::move(expression)) {}

Statement::Statement(ExpressionStatement expression)
    : kind(Kind::ExpressionStatement), expression(std::move(expression)) {}

Statement::~Statement()
{
    switch (kind)
    {
    case Kind::ExpressionStatement: expression.~ExpressionStatement(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
