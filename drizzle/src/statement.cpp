#include "statement.h"

#include <shell/macros.h>

Statement::Block::Block(std::string_view identifier, std::vector<Stmt> statements)
    : identifier(identifier), statements(std::move(statements)) {}

Statement::ExpressionStatement::ExpressionStatement(Expr expression)
    : expression(std::move(expression)) {}

Statement::Statement(ExpressionStatement expression)
    : type(Type::ExpressionStatement), expression_statement(std::move(expression)) {}

Statement::Statement(Block block)
    : type(Type::Block), block(std::move(block)) {}

Statement::Statement(Program program)
    : type(Type::Program), program(std::move(program)) {}

Statement::Program::Program(std::vector<Stmt> statements)
    : statements(std::move(statements)) {}

Statement::~Statement()
{
    switch (type)
    {
    case Type::Block: block.~Block(); break;
    case Type::ExpressionStatement: expression_statement.~ExpressionStatement(); break;
    case Type::Program: program.~Program(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
