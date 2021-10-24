#include "expression.h"

#include <shell/macros.h>

Expression::Binary::Binary(Type type, Expr left, Expr right)
    : type(type), left(std::move(left)), right(std::move(right)) {}

Expression::Group::Group(Expr expression)
    : expression(std::move(expression)) {}

Expression::Literal::Literal()
    : type(Type::Null) {}

Expression::Literal::Literal(dzbool value)
    : type(Type::Boolean), value(value) {}

Expression::Literal::Literal(dzint value)
    : type(Type::Integer), value(value) {}

Expression::Literal::Literal(dzfloat value)
    : type(Type::Float), value(value) {}

Expression::Literal::Literal(const std::string& value)
    : type(Type::String), value(value) {}

Expression::Unary::Unary(Type type, Expr expression)
    : type(type), expression(std::move(expression)) {}

Expression::Expression(Binary binary)
    : type(Type::Binary), binary(std::move(binary)) {}

Expression::Expression(Group group)
    : type(Type::Group), group(std::move(group)) {}

Expression::Expression(Literal literal)
    : type(Type::Literal), literal(std::move(literal)) {}

Expression::Expression(Unary unary)
    : type(Type::Unary), unary(std::move(unary)) {}

Expression::~Expression()
{
    switch (type)
    {
    case Type::Binary: binary.~Binary(); break;
    case Type::Group: group.~Group(); break;
    case Type::Literal: literal.~Literal(); break;
    case Type::Unary: unary.~Unary(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
