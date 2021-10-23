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
        ExpressionStatement,
        Program,
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

    Statement(ExpressionStatement expression);
    Statement(Program program);
    ~Statement();

    Type type;
    union
    {
        ExpressionStatement expression;
        Program program;
    };
};
