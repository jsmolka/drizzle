#include "statement.h"

#include <shell/macros.h>

Statement::ExpressionStatement::ExpressionStatement(Expr expression)
    : expression(std::move(expression)) {}

Statement::Statement(ExpressionStatement expression)
    : type(Type::ExpressionStatement), expression(std::move(expression)) {}

Statement::Statement(Program program)
    : type(Type::Program), program(std::move(program)) {}

Statement::Program::Program(std::vector<Stmt> statements)
    : statements(std::move(statements)) {}

Statement::~Statement()
{
    switch (type)
    {
    case Type::ExpressionStatement: expression.~ExpressionStatement(); break;
    case Type::Program: program.~Program(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
