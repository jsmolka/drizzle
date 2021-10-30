#include "statement.h"

#include <shell/macros.h>

Statement::Block::Block(std::string_view identifier, Stmts statements)
    : identifier(identifier), statements(std::move(statements))
{
}

Statement::ExpressionStatement::ExpressionStatement(Expr expression)
    : expression(std::move(expression))
{
}

Statement::If::Branch::Branch(Expr condition, Stmts statements)
    : condition(std::move(condition)), statements(std::move(statements))
{
}

Statement::If::If(Branch if_, std::vector<Branch> elifs, Stmts else_)
    : if_(std::move(if_)), elifs(std::move(elifs)), else_(std::move(else_))
{
}

Statement::Print::Print(Expr expression)
    : expression(std::move(expression))
{
}

Statement::Program::Program(Stmts statements)
    : statements(std::move(statements))
{
}

Statement::Var::Var(std::string_view identifier, Expr initializer)
    : identifier(identifier), initializer(std::move(initializer))
{
}

Statement::While::While(Expr condition, Stmts statements)
    : condition(std::move(condition)), statements(std::move(statements))
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

Statement::Statement(If if_, const SourceLocation& location)
    : type(Type::If), if_(std::move(if_)), location(location)
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

Statement::Statement(Var var, const SourceLocation& location)
    : type(Type::Var), var(std::move(var)), location(location)
{
}

Statement::Statement(While while_, const SourceLocation& location)
    : type(Type::While), while_(std::move(while_)), location(location)
{
}

Statement::~Statement()
{
    switch (type)
    {
    case Type::Block: block.~Block(); break;
    case Type::ExpressionStatement: expression_statement.~ExpressionStatement(); break;
    case Type::If: if_.~If(); break;
    case Type::Noop: break;
    case Type::Print: print.~Print(); break;
    case Type::Program: program.~Program(); break;
    case Type::Var: var.~Var(); break;
    case Type::While: while_.~While(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
