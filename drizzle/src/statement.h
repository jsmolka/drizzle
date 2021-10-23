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
        Program,
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

    struct Program
    {
        Program(std::vector<Stmt> statements);

        std::vector<Stmt> statements;
    };

    Statement(Block block);
    Statement(ExpressionStatement expression);
    Statement(Program program);
    ~Statement();

    Type type;
    union
    {
        Block block;
        ExpressionStatement expression;
        Program program;
    };
};

template<typename T, typename... Args>
Stmt newStmt(Args... args)
{
    return std::make_unique<Statement>(T(std::forward<Args>(args)...));
}
