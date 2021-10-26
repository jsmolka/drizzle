#pragma once

#include <vector>

#include "expression.h"

class Statement;

using Stmt = std::shared_ptr<Statement>;

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
        LastEnumValue,
    };

    struct Block
    {
        Block(std::string_view identifier, std::vector<Stmt> statements);

        std::string_view identifier;
        std::vector<Stmt> statements;
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
        Program(std::vector<Stmt> statements);

        std::vector<Stmt> statements;
    };

    Statement(const SourceLocation& location);
    Statement(Block block, const SourceLocation& location);
    Statement(ExpressionStatement expression, const SourceLocation& location);
    Statement(Print print, const SourceLocation& location);
    Statement(Program program, const SourceLocation& location);
    ~Statement();

    const Type type;
    union
    {
        Block block;
        ExpressionStatement expression_statement;
        Print print;
        Program program;
    };
    const SourceLocation location;
};
