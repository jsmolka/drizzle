#include "statement.h"

#include <shell/macros.h>

Statement::ExpressionStatement::ExpressionStatement(Expr expression)
    : expression(std::move(expression)) {}

Statement::Statement(ExpressionStatement expression)
    : kind(Kind::ExpressionStatement), expression(std::move(expression)) {}

Statement::Statement(Program program)
    : kind(Kind::Program), program(std::move(program)) {}

Statement::Program::Program(std::vector<Stmt> statements)
    : statements(std::move(statements)) {}

Statement::~Statement()
{
    switch (kind)
    {
    case Kind::ExpressionStatement: expression.~ExpressionStatement(); break;
    case Kind::Program: program.~Program(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
