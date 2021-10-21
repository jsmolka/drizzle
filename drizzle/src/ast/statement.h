#pragma once

#include <memory>

class Expression;

class Statement
{
public:
    enum class Kind
    {
        ExpressionStatement,
    };

    struct ExpressionStatement
    {
        ExpressionStatement(std::unique_ptr<Expression> expression);

        std::unique_ptr<Expression> expression;
    };

    Statement(ExpressionStatement expression);
    ~Statement();

    Kind kind;
    union
    {
        ExpressionStatement expression;
    };
};
