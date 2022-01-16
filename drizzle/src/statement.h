#pragma once

#include <vector>

#include "expression.h"

class Statement;

using Stmt  = std::unique_ptr<Statement>;
using Stmts = std::vector<Stmt>;

class Statement {
public:
  enum class Type {
    Block,
    Break,
    Continue,
    ExpressionStatement,
    If,
    Noop,
    Print,
    Program,
    Var,
    While,
    LastEnumValue,
  };

  struct Block {
    Block(std::string_view identifier, Stmts statements);

    std::string_view identifier;
    Stmts statements;
  };

  struct Break {
    Break(std::string_view identifier);

    std::string_view identifier;
  };

  struct Continue {};

  struct ExpressionStatement {
    ExpressionStatement(Expr expression);

    Expr expression;
  };

  struct If {
    struct Branch {
      Branch(Expr condition, Stmts statements);

      Expr condition;
      Stmts statements;
    };

    If(Branch if_, std::vector<Branch> elifs, Stmts else_);

    Branch if_;
    std::vector<Branch> elifs;
    Stmts else_;
  };

  struct Noop {};

  struct Print {
    Print(Expr expression);

    Expr expression;
  };

  struct Program {
    Program(Stmts statements);

    Stmts statements;
  };

  struct Var {
    Var(std::string_view identifier, Expr initializer);

    std::string_view identifier;
    Expr initializer;
  };

  struct While {
    While(Expr condition, Stmts statements);

    Expr condition;
    Stmts statements;
  };

  Statement(Block block, const Location& location);
  Statement(Break break_, const Location& location);
  Statement(Continue continue_, const Location& location);
  Statement(ExpressionStatement expression, const Location& location);
  Statement(If if_, const Location& location);
  Statement(Noop noop, const Location& location);
  Statement(Print print, const Location& location);
  Statement(Program program, const Location& location);
  Statement(Var var, const Location& location);
  Statement(While while_, const Location& location);
  ~Statement();

  const Type type;
  union {
    Block block;
    Break break_;
    Continue continue_;
    ExpressionStatement expression_statement;
    If if_;
    Noop noop;
    Print print;
    Program program;
    Var var;
    While while_;
  };
  const Location location;
};

template<>
struct fmt::formatter<Statement::Type> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Statement::Type& value, FormatContext& ctx) {
    auto repr = [](const Statement::Type& value) {
      static_assert(int(Statement::Type::LastEnumValue) == 10);
      switch (value) {
        case Statement::Type::Block:               return "block";
        case Statement::Type::Break:               return "break";
        case Statement::Type::Continue:            return "continue";
        case Statement::Type::ExpressionStatement: return "expression_statement";
        case Statement::Type::If:                  return "if";
        case Statement::Type::Noop:                return "noop";
        case Statement::Type::Print:               return "print";
        case Statement::Type::Program:             return "program";
        case Statement::Type::Var:                 return "var";
        case Statement::Type::While:               return "while";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};
