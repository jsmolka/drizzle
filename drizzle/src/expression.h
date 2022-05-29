#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <sh/utility.h>

#include "dzprimitives.h"
#include "identifier.h"

class Expression;

using Expr  = std::unique_ptr<Expression>;
using Exprs = std::vector<Expr>;

class Expression {
public:
  enum class Type {
    Assign,
    Binary,
    Call,
    Get,
    Group,
    In,
    Invoke,
    List,
    Literal,
    Range,
    Set,
    SubscriptGet,
    SubscriptSet,
    Unary,
    Variable,
    LastEnumValue,
  };

  struct Assign {
    Identifier identifier;
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

    Type type;
    Expr left;
    Expr right;
  };

  struct Call {
    Expr callee;
    Exprs arguments;
  };

  struct Get {
    Identifier identifier;
    Expr self;
  };

  struct Group {
    Expr expression;
  };

  struct In {
    Expr self;
    Expr expression;
  };

  struct Invoke {
    Identifier identifier;
    Expr self;
    Exprs arguments;
  };

  struct List {
    Exprs values;
  };

  struct Literal {
    enum class Type {
      Boolean,
      Integer,
      Float,
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

  struct Range {
    Expr start;
    Expr stop;
  };

  struct Set {
    Identifier identifier;
    Expr self;
    Expr value;
  };

  struct SubscriptGet {
    Expr self;
    Expr expression;
  };

  struct SubscriptSet {
    Expr self;
    Expr expression;
    Expr value;
  };

  struct Unary {
    enum class Type {
      BitwiseComplement,
      Minus,
      Not,
      LastEnumValue,
    };

    Type type;
    Expr expression;
  };

  struct Variable {
    Identifier identifier;
  };

  Expression(Assign assign, const Location& location);
  Expression(Binary binary, const Location& location);
  Expression(Call call, const Location& location);
  Expression(Get get, const Location& location);
  Expression(Group group, const Location& location);
  Expression(In in, const Location& location);
  Expression(Invoke invoke, const Location& location);
  Expression(List list, const Location& location);
  Expression(Literal literal, const Location& location);
  Expression(Range range, const Location& location);
  Expression(Set set, const Location& location);
  Expression(SubscriptGet subscript_get, const Location& location);
  Expression(SubscriptSet subscript_set, const Location& location);
  Expression(Unary unary, const Location& location);
  Expression(Variable variable, const Location& location);
  ~Expression();

  Type type;
  union {
    Assign assign;
    Binary binary;
    Call call;
    Get get;
    Group group;
    In in;
    Invoke invoke;
    List list;
    Literal literal;
    Range range;
    Set set;
    SubscriptGet subscript_get;
    SubscriptSet subscript_set;
    Unary unary;
    Variable variable;
  };
  Location location;
};

template<>
struct fmt::formatter<Expression::Type> : fmt::formatter<std::string_view> {
  static auto repr(const Expression::Type& value) -> std::string_view {
    static_assert(int(Expression::Type::LastEnumValue) == 15);
    switch (value) {
      case Expression::Type::Assign:       return "assign";
      case Expression::Type::Binary:       return "binary";
      case Expression::Type::Call:         return "call";
      case Expression::Type::Get:          return "get";
      case Expression::Type::Group:        return "group";
      case Expression::Type::In:           return "in";
      case Expression::Type::Invoke:       return "invoke";
      case Expression::Type::List:         return "list";
      case Expression::Type::Literal:      return "literal";
      case Expression::Type::Range:        return "range";
      case Expression::Type::Set:          return "set";
      case Expression::Type::SubscriptGet: return "subscript_get";
      case Expression::Type::SubscriptSet: return "subscript_set";
      case Expression::Type::Unary:        return "unary";
      case Expression::Type::Variable:     return "variable";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }
  
  template<typename FormatContext>
  auto format(const Expression::Type& value, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<Expression::Binary::Type> : fmt::formatter<std::string_view> {
  static auto repr(const Expression::Binary::Type& value) -> std::string_view {
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
  }

  
  template<typename FormatContext>
  auto format(const Expression::Binary::Type& value, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<Expression::Unary::Type> : fmt::formatter<std::string_view> {
  static auto repr(const Expression::Unary::Type& value) -> std::string_view {
    static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);
    switch (value) {
      case Expression::Unary::Type::BitwiseComplement: return "~";
      case Expression::Unary::Type::Minus:             return "-";
      case Expression::Unary::Type::Not:               return "!";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }
  
  template<typename FormatContext>
  auto format(const Expression::Unary::Type& value, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};
