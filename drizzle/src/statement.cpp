#include "statement.h"

#include <shell/macros.h>

Statement::Block::Block(std::string_view identifier, std::vector<Stmt> statements)
    : identifier(identifier), statements(std::move(statements))
{
}

Statement::ExpressionStatement::ExpressionStatement(Expr expression)
    : expression(std::move(expression))
{
}

Statement::Print::Print(Expr expression)
    : expression(std::move(expression))
{
}

Statement::Program::Program(std::vector<Stmt> statements)
    : statements(std::move(statements))
{
}

Statement::Statement(const SourceLocation& location)
    : type(Type::Noop), location(location)
{
}

Statement::Statement(ExpressionStatement expression, const SourceLocation& location)
    : type(Type::ExpressionStatement), expression_statement(std::move(expression)), location(location)
{
}

Statement::Statement(Print print, const SourceLocation& location)
    : type(Type::Print), print(std::move(print)), location(location)
{
}

Statement::Statement(Block block, const SourceLocation& location)
    : type(Type::Block), block(std::move(block)), location(location)
{
}

Statement::Statement(Program program, const SourceLocation& location)
    : type(Type::Program), program(std::move(program)), location(location)
{
}

Statement::~Statement()
{
    switch (type)
    {
    case Type::Block: block.~Block(); break;
    case Type::ExpressionStatement: expression_statement.~ExpressionStatement(); break;
    case Type::Noop: break;
    case Type::Print: print.~Print(); break;
    case Type::Program: program.~Program(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
