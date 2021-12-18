#include "expression.h"

#include <sh/utility.h>

Expression::Assign::Assign(std::string_view identifier, Expr value)
    : identifier(identifier), value(std::move(value)) {}

Expression::Binary::Binary(Type type, Expr left, Expr right)
    : type(type), left(std::move(left)), right(std::move(right)) {}

Expression::Group::Group(Expr expression) : expression(std::move(expression)) {}

Expression::Literal::Literal() : type(Type::Null) {}

Expression::Literal::Literal(dzbool value) : type(Type::Boolean), value(value) {}

Expression::Literal::Literal(dzint value) : type(Type::Integer), value(value) {}

Expression::Literal::Literal(dzfloat value) : type(Type::Float), value(value) {}

Expression::Literal::Literal(const std::string& value) : type(Type::String), value(value) {}

Expression::Unary::Unary(Type type, Expr expression)
    : type(type), expression(std::move(expression)) {}

Expression::Variable::Variable(std::string_view identifier) : identifier(identifier) {}

Expression::Expression(Assign assign, const SourceLocation& location)
    : type(Type::Assign), assign(std::move(assign)), location(location) {}

Expression::Expression(Binary binary, const SourceLocation& location)
    : type(Type::Binary), binary(std::move(binary)), location(location) {}

Expression::Expression(Group group, const SourceLocation& location)
    : type(Type::Group), group(std::move(group)), location(location) {}

Expression::Expression(Literal literal, const SourceLocation& location)
    : type(Type::Literal), literal(std::move(literal)), location(location) {}

Expression::Expression(Unary unary, const SourceLocation& location)
    : type(Type::Unary), unary(std::move(unary)), location(location) {}

Expression::Expression(Variable variable, const SourceLocation& location)
    : type(Type::Variable), variable(std::move(variable)), location(location) {}

Expression::~Expression() {
  switch (type) {
    case Type::Assign:
      assign.~Assign();
      break;
    case Type::Binary:
      binary.~Binary();
      break;
    case Type::Group:
      group.~Group();
      break;
    case Type::Literal:
      literal.~Literal();
      break;
    case Type::Unary:
      unary.~Unary();
      break;
    case Type::Variable:
      variable.~Variable();
      break;

    default:
      SH_UNREACHABLE;
      break;
  }
}
