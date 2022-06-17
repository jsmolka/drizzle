#pragma once

#include <optional>
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
    Class,
    Continue,
    Def,
    ExpressionStatement,
    For,
    If,
    Noop,
    Program,
    Return,
    Switch,
    Var,
    While,
    LastEnumValue,
  };

  struct Block {
    std::optional<Identifier> identifier;
    Stmts statements;
  };

  struct Break {
    std::optional<Identifier> identifier;
  };

  struct Class {
    Identifier identifier;
    Stmts methods;
  };

  struct Continue {};

  struct Def {
    Identifier identifier;
    std::vector<Identifier> parameters;
    Stmts statements;
  };

  struct ExpressionStatement {
    Expr expression;
  };

  struct For {
    Identifier iterator;
    Expr iteree;
    Stmts statements;
  };

  struct If {
    struct Branch {
      Expr condition;
      Stmts statements;
    };

    std::vector<Branch> branches;
    std::optional<Stmts> else_;
  };

  struct Noop {};

  struct Program {
    Stmts statements;
  };

  struct Return {
    std::optional<Expr> expression;
  };

  struct Switch {
    struct Case {
      Expr value;
      Stmts statements;
    };

    Expr value;
    std::vector<Case> cases;
    std::optional<Stmts> default_;
  };

  struct Var {
    Identifier identifier;
    Expr initializer;
  };

  struct While {
    Expr condition;
    Stmts statements;
  };

  Statement(Block block, const Location& location);
  Statement(Break break_, const Location& location);
  Statement(Class class_, const Location& location);
  Statement(Continue continue_, const Location& location);
  Statement(Def define, const Location& location);
  Statement(ExpressionStatement expression, const Location& location);
  Statement(For for_, const Location& location);
  Statement(If if_, const Location& location);
  Statement(Noop noop, const Location& location);
  Statement(Program program, const Location& location);
  Statement(Return return_, const Location& location);
  Statement(Switch switch_, const Location& location);
  Statement(Var var, const Location& location);
  Statement(While while_, const Location& location);
  ~Statement();

  Type type;
  union {
    Block block;
    Break break_;
    Class class_;
    Continue continue_;
    Def def;
    ExpressionStatement expression_statement;
    For for_;
    If if_;
    Noop noop;
    Program program;
    Return return_;
    Switch switch_;
    Var var;
    While while_;
  };
  Location location;
};

template<>
struct fmt::formatter<Statement::Type> : fmt::formatter<std::string_view> {
  static auto repr(const Statement::Type& value) -> std::string_view {
    static_assert(int(Statement::Type::LastEnumValue) == 14);
    switch (value) {
      case Statement::Type::Block:               return "block";
      case Statement::Type::Break:               return "break";
      case Statement::Type::Class:               return "class";
      case Statement::Type::Continue:            return "continue";
      case Statement::Type::Def:                 return "def";
      case Statement::Type::ExpressionStatement: return "expression_statement";
      case Statement::Type::For:                 return "for";
      case Statement::Type::If:                  return "if";
      case Statement::Type::Noop:                return "noop";
      case Statement::Type::Program:             return "program";
      case Statement::Type::Return:              return "return";
      case Statement::Type::Switch:              return "switch";
      case Statement::Type::Var:                 return "var";
      case Statement::Type::While:               return "while";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }
  
  template<typename FormatContext>
  auto format(const Statement::Type& value, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};
