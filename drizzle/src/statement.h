#pragma once

#include <vector>

#include "expression.h"

class Statement;

using Stmt  = std::shared_ptr<Statement>;
using Stmts = std::vector<Stmt>;

class Statement
{
public:
    enum class Type
    {
        Block,
        ExpressionStatement,
        Noop,
        Print,
        Program,
        Var,
        LastEnumValue,
    };

    struct Block
    {
        Block(std::string_view identifier, Stmts statements);

        std::string_view identifier;
        Stmts statements;
    };

    struct ExpressionStatement
    {
        ExpressionStatement(Expr expression);

        Expr expression;
    };

    struct Print
    {
        Print(Expr expression);

        Expr expression;
    };

    struct Program
    {
        Program(Stmts statements);

        Stmts statements;
    };

    struct Var
    {
        Var(std::string_view identifier, Expr initializer);

        std::string_view identifier;
        Expr initializer;
    };

    Statement(const SourceLocation& location);
    Statement(Block block, const SourceLocation& location);
    Statement(ExpressionStatement expression, const SourceLocation& location);
    Statement(Print print, const SourceLocation& location);
    Statement(Program program, const SourceLocation& location);
    Statement(Var var, const SourceLocation& location);
    ~Statement();

    const Type type;
    union
    {
        Block block;
        ExpressionStatement expression_statement;
        Print print;
        Program program;
        Var var;
    };
    const SourceLocation location;
};
