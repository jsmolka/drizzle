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
        If,
        Noop,
        Print,
        Program,
        Var,
        While,
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

    struct If
    {
        struct Branch
        {
            Branch(Expr condition, Stmts statements);

            Expr condition;
            Stmts statements;
        };

        If(Branch if_, std::vector<Branch> elifs, Stmts else_);

        Branch if_;
        std::vector<Branch> elifs;
        Stmts else_;
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

    struct While
    {
        While(Expr condition, Stmts statements);

        Expr condition;
        Stmts statements;
    };

    Statement(const SourceLocation& location);
    Statement(Block block, const SourceLocation& location);
    Statement(ExpressionStatement expression, const SourceLocation& location);
    Statement(If if_, const SourceLocation& location);
    Statement(Print print, const SourceLocation& location);
    Statement(Program program, const SourceLocation& location);
    Statement(Var var, const SourceLocation& location);
    Statement(While while_, const SourceLocation& location);
    ~Statement();

    const Type type;
    union
    {
        Block block;
        ExpressionStatement expression_statement;
        If if_;
        Print print;
        Program program;
        Var var;
        While while_;
    };
    const SourceLocation location;
};
