#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "dzprimitives.h"
#include "sourcelocation.h"

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

  Expression(Assign assign, const SourceLocation& location);
  Expression(Binary binary, const SourceLocation& location);
  Expression(Group group, const SourceLocation& location);
  Expression(Literal literal, const SourceLocation& location);
  Expression(Unary unary, const SourceLocation& location);
  Expression(Variable variable, const SourceLocation& location);
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
