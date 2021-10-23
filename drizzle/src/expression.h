#pragma once

#include <memory>
#include <string>
#include <variant>

#include "dzprimitives.h"

class Expression;

using Expr = std::unique_ptr<Expression>;

class Expression
{
public:
    enum class Type
    {
        Binary,
        Group,
        Literal,
        Unary,
    };

    struct Binary
    {
        enum class Type
        {
            And,
            Addition,
            BitwiseAnd,
            BitwiseAsr,
            BitwiseLsl,
            BitwiseLsr,
            BitwiseOr,
            BitwiseXor,
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
            LastEnumValue,
        };

        Binary(Type type, Expr left, Expr right);

        Type type;
        Expr left;
        Expr right;
    };

    struct Group
    {
        Group(Expr expression);

        Expr expression;
    };

    struct Literal
    {
        enum class Type
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

        Type type;
        std::variant<dzbool, dzint, dzfloat, std::string> value;
    };

    struct Unary
    {
        enum class Type
        {
            BitwiseComplement,
            Minus,
            Not,
        };

        Unary(Type type, Expr expression);

        Type type;
        Expr expression;
    };

    Expression(Binary binary);
    Expression(Group group);
    Expression(Literal literal);
    Expression(Unary unary);
    ~Expression();

    Type type;
    union
    {
        Binary binary;
        Group group;
        Literal literal;
        Unary unary;
    };
};

template<typename T, typename... Args>
Expr newExpr(Args... args)
{
    return std::make_unique<Expression>(T(std::forward<Args>(args)...));
}
