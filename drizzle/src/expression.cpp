#include "expression.h"

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

auto Expression::Literal::repr() const -> std::string {
  switch (type) {
    case Type::Boolean: return fmt::to_string(std::get<dzbool>(value));
    case Type::Integer: return fmt::to_string(std::get<dzint>(value));
    case Type::Float:   return fmt::to_string(std::get<dzfloat>(value));
    case Type::Null:    return "null";
    case Type::String:  return fmt::format(R"("{}")", std::get<std::string>(value));
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

Expression::Expression(Assign assign, const Location& location)
  : type(Type::Assign), assign(std::move(assign)), location(location) {}

Expression::Expression(Binary binary, const Location& location)
  : type(Type::Binary), binary(std::move(binary)), location(location) {}

Expression::Expression(BracketGet bracket_get, const Location& location)
  : type(Type::BracketGet), bracket_get(std::move(bracket_get)), location(location) {}

Expression::Expression(BracketSet bracket_set, const Location& location)
  : type(Type::BracketSet), bracket_set(std::move(bracket_set)), location(location) {}

Expression::Expression(Call call, const Location& location)
  : type(Type::Call), call(std::move(call)), location(location) {}

Expression::Expression(Get get, const Location& location)
  : type(Type::Get), get(std::move(get)), location(location) {}

Expression::Expression(Group group, const Location& location)
  : type(Type::Group), group(std::move(group)), location(location) {}

Expression::Expression(Invoke invoke, const Location& location)
  : type(Type::Invoke), invoke(std::move(invoke)), location(location) {}

Expression::Expression(List list, const Location& location)
  : type(Type::List), list(std::move(list)), location(location) {}

Expression::Expression(Literal literal, const Location& location)
  : type(Type::Literal), literal(std::move(literal)), location(location) {}

Expression::Expression(Set set, const Location& location)
  : type(Type::Set), set(std::move(set)), location(location) {}

Expression::Expression(Unary unary, const Location& location)
  : type(Type::Unary), unary(std::move(unary)), location(location) {}

Expression::Expression(Variable variable, const Location& location)
  : type(Type::Variable), variable(std::move(variable)), location(location) {}

Expression::~Expression() {
  switch (type) {
    case Type::Assign:     std::destroy_at(&assign); break;
    case Type::Binary:     std::destroy_at(&binary); break;
    case Type::BracketGet: std::destroy_at(&bracket_get); break;
    case Type::BracketSet: std::destroy_at(&bracket_set); break;
    case Type::Call:       std::destroy_at(&call); break;
    case Type::Get:        std::destroy_at(&get); break;
    case Type::Group:      std::destroy_at(&group); break;
    case Type::Invoke:     std::destroy_at(&invoke); break;
    case Type::List:       std::destroy_at(&list); break;
    case Type::Literal:    std::destroy_at(&literal); break;
    case Type::Set:        std::destroy_at(&set); break;
    case Type::Unary:      std::destroy_at(&unary); break;
    case Type::Variable:   std::destroy_at(&variable); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}
