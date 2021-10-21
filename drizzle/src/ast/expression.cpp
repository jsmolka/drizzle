#include "expression.h"

#include <shell/macros.h>

Expression::Binary::Binary(Kind kind, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
    : kind(kind), left(std::move(left)), right(std::move(right)) {}

Expression::Group::Group(std::unique_ptr<Expression> expression)
    : expression(std::move(expression)) {}

Expression::Literal::Literal()
    : kind(Kind::Null) {}

Expression::Literal::Literal(dzbool value)
    : kind(Kind::Boolean), value(value) {}

Expression::Unary::Unary(Kind kind, std::unique_ptr<Expression> expression)
    : kind(kind), expression(std::move(expression)) {}

Expression::Literal::Literal(dzint value)
    : kind(Kind::Integer), value(value) {}

Expression::Literal::Literal(dzfloat value)
    : kind(Kind::Float), value(value) {}

Expression::Literal::Literal(const std::string& value)
    : kind(Kind::String), value(value) {}

Expression::Expression(Binary binary)
    : kind(Kind::Binary), binary(std::move(binary)) {}

Expression::Expression(Group group)
    : kind(Kind::Group), group(std::move(group)) {}

Expression::Expression(Literal literal)
    : kind(Kind::Literal), literal(std::move(literal)) {}

Expression::Expression(Unary unary)
    : kind(Kind::Unary), unary(std::move(unary)) {}

Expression::~Expression()
{
    switch (kind)
    {
    case Kind::Binary: binary.~Binary(); break;
    case Kind::Group: group.~Group(); break;
    case Kind::Literal: literal.~Literal(); break;
    case Kind::Unary: unary.~Unary(); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
