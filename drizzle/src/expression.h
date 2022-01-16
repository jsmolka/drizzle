#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzprimitives.h"
#include "location.h"

class Expression;

using Expr  = std::unique_ptr<Expression>;
using Exprs = std::vector<Expr>;

class Expression {
public:
  enum class Type {
    Assign,
    Binary,
    Group,
    Literal,
    Unary,
    Variable,
    LastEnumValue,
  };

  struct Assign {
    Assign(std::string_view identifier, Expr value);

    std::string_view identifier;
    Expr value;
  };

  struct Binary {
    enum class Type {
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

  struct Group {
    Group(Expr expression);

    Expr expression;
  };

  struct Literal {
    enum class Type {
      Boolean,
      Float,
      Integer,
      Null,
      String,
      LastEnumValue,
    };

    Literal();
    Literal(dzbool value);
    Literal(dzint value);
    Literal(dzfloat value);
    Literal(const std::string& value);

    auto repr() const -> std::string;

    Type type;
    std::variant<dzbool, dzint, dzfloat, std::string> value;
  };

  struct Unary {
    enum class Type {
      BitwiseComplement,
      Minus,
      Not,
      LastEnumValue,
    };

    Unary(Type type, Expr expression);

    Type type;
    Expr expression;
  };

  struct Variable {
    Variable(std::string_view identifier);

    std::string_view identifier;
  };

  Expression(Assign assign, const Location& location);
  Expression(Binary binary, const Location& location);
  Expression(Group group, const Location& location);
  Expression(Literal literal, const Location& location);
  Expression(Unary unary, const Location& location);
  Expression(Variable variable, const Location& location);
  ~Expression();

  const Type type;
  union {
    Assign assign;
    Binary binary;
    Group group;
    Literal literal;
    Unary unary;
    Variable variable;
  };
  const Location location;
};

template<>
struct fmt::formatter<Expression::Type> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Expression::Type& value, FormatContext& ctx) const {
    auto repr = [](const Expression::Type& value) {
      static_assert(int(Expression::Type::LastEnumValue) == 6);
      switch (value) {
        case Expression::Type::Assign:   return "assign";
        case Expression::Type::Binary:   return "binary";
        case Expression::Type::Group:    return "group";
        case Expression::Type::Literal:  return "literal";
        case Expression::Type::Unary:    return "unary";
        case Expression::Type::Variable: return "variable";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<Expression::Binary::Type> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Expression::Binary::Type& value, FormatContext& ctx) const {
    auto repr = [](const Expression::Binary::Type& value) {
      static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);
      switch (value) {
        case Expression::Binary::Type::Addition:        return "+";
        case Expression::Binary::Type::And:             return "&&";
        case Expression::Binary::Type::BitwiseAnd:      return "&";
        case Expression::Binary::Type::BitwiseAsr:      return ">>";
        case Expression::Binary::Type::BitwiseLsl:      return "<<";
        case Expression::Binary::Type::BitwiseLsr:      return ">>>";
        case Expression::Binary::Type::BitwiseOr:       return "|";
        case Expression::Binary::Type::BitwiseXor:      return "^";
        case Expression::Binary::Type::Division:        return "/";
        case Expression::Binary::Type::Equal:           return "==";
        case Expression::Binary::Type::Greater:         return ">";
        case Expression::Binary::Type::GreaterEqual:    return ">=";
        case Expression::Binary::Type::IntegerDivision: return "//";
        case Expression::Binary::Type::Less:            return "<";
        case Expression::Binary::Type::LessEqual:       return "<=";
        case Expression::Binary::Type::Modulo:          return "%";
        case Expression::Binary::Type::Multiplication:  return "*";
        case Expression::Binary::Type::NotEqual:        return "!=";
        case Expression::Binary::Type::Or:              return "||";
        case Expression::Binary::Type::Power:           return "**";
        case Expression::Binary::Type::Subtraction:     return "-";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<Expression::Unary::Type> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Expression::Unary::Type& value, FormatContext& ctx) const {
    auto repr = [](const Expression::Unary::Type& value) {
      static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);
      switch (value) {
        case Expression::Unary::Type::BitwiseComplement: return "~";
        case Expression::Unary::Type::Minus:             return "-";
        case Expression::Unary::Type::Not:               return "!";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};
