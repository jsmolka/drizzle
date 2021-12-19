#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "dzprimitives.h"
#include "sourcelocation.h"

class Expression;

using Expr = std::unique_ptr<Expression>;
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
    Assign(std::string_view identifier, Expr value)
        : identifier(identifier), value(std::move(value)) {}

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

    Binary(Type type, Expr left, Expr right)
        : type(type), left(std::move(left)), right(std::move(right)) {}

    Type type;
    Expr left;
    Expr right;
  };

  struct Group {
    Group(Expr expression) : expression(std::move(expression)) {}

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

    Literal() : type(Type::Null) {}
    Literal(dzbool value) : type(Type::Boolean), value(value) {}
    Literal(dzint value) : type(Type::Integer), value(value) {}
    Literal(dzfloat value) : type(Type::Float), value(value) {}
    Literal(const std::string& value) : type(Type::String), value(value) {}

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

    Unary(Type type, Expr expression) : type(type), expression(std::move(expression)) {}

    Type type;
    Expr expression;
  };

  struct Variable {
    Variable(std::string_view identifier) : identifier(identifier) {}

    std::string_view identifier;
  };

  Expression(Assign assign, const SourceLocation& location)
      : type(Type::Assign), assign(std::move(assign)), location(location) {}
  Expression(Binary binary, const SourceLocation& location)
      : type(Type::Binary), binary(std::move(binary)), location(location) {}
  Expression(Group group, const SourceLocation& location)
      : type(Type::Group), group(std::move(group)), location(location) {}
  Expression(Literal literal, const SourceLocation& location)
      : type(Type::Literal), literal(std::move(literal)), location(location) {}
  Expression(Unary unary, const SourceLocation& location)
      : type(Type::Unary), unary(std::move(unary)), location(location) {}
  Expression(Variable variable, const SourceLocation& location)
      : type(Type::Variable), variable(std::move(variable)), location(location) {}
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
  const SourceLocation location;
};
