#pragma once

#include <memory>
#include <string>
#include <variant>

#include "dzprimitives.h"

class Expression
{
public:
    enum class Kind
    {
        Binary,
        Group,
        Literal,
        Unary,
    };

    struct Binary
    {
        enum class Kind
        {
            And,
            Addition,
            BitwiseAnd,
            BitwiseAsr,
            BitwiseLsl,
            BitwiseLsr,
            BitwiseOr,
            Division,
            Equal,
            Greater,
            GreaterEqual,
            IntegerDivision,
            Less,
            LessEqual,
            Modulo,
            Multiplication,
            NotEqual,
            Or,
            Power,
            Subtraction,
        };

        Binary(Kind kind, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

        Kind kind;
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
    };

    struct Group
    {
        Group(std::unique_ptr<Expression> expression);

        std::unique_ptr<Expression> expression;
    };

    struct Literal
    {
        enum class Kind
        {
            Boolean,
            Float,
            Integer,
            Null,
            String,
        };

        Literal();
        Literal(dzbool value);
        Literal(dzint value);
        Literal(dzfloat value);
        Literal(const std::string& value);

        Kind kind;
        std::variant<dzbool, dzint, dzfloat, std::string> value;
    };

    struct Unary
    {
        enum class Kind
        {
            BitwiseComplement,
            Minus,
            Not,
        };

        Unary(Kind kind, std::unique_ptr<Expression> expression);

        Kind kind;
        std::unique_ptr<Expression> expression;
    };

    Expression(Binary binary);
    Expression(Group group);
    Expression(Literal literal);
    Expression(Unary unary);
    ~Expression();

    Kind kind;
    union
    {
        Binary binary;
        Group group;
        Literal literal;
        Unary unary;
    };
};
