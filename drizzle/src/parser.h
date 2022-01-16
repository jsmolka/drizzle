#pragma once

#include <sh/stack.h>

#include "statement.h"
#include "token.h"

class Parser {
public:
  auto parse(const std::vector<Token>& tokens) -> Stmt;

private:
  enum class Precedence {
    None,
    Assignment,
    Or,
    And,
    BitOr,
    BitXor,
    BitAnd,
    Equality,
    Comparison,
    BitShift,
    Term,
    Factor,
    Unary,
    Primary,
  };

  struct Rule {
    using Parselet = void (Parser::*)(bool);

    Parselet prefix;
    Parselet infix;
    Precedence precedence;
  };

  static auto rule(Token::Type type) -> const Parser::Rule&;

  void advance();
  auto match(Token::Type type) -> bool;
  void expect(Token::Type type, std::string_view error);
  void expectColon();
  void expectDedent();
  void expectIdentifier();
  void expectIndent();
  void expectNewLine();
  void expectParenLeft();
  void expectParenRight();

  template<typename T, typename... Args>
    requires std::constructible_from<T, Args...>
  auto newExpr(Args... args) -> Expr;
  auto expression() -> Expr;
  void parseExpression(Precedence precedence);
  void and_(bool);
  void binary(bool);
  void constant(bool);
  void group(bool);
  void literal(bool);
  void or_(bool);
  void unary(bool);
  void variable(bool assign);

  template<typename T>
    requires std::constructible_from<Statement, T, Location>
  auto newStmt(T statement) -> Stmt;
  void pushLocation();
  auto program() -> Stmt;
  auto declaration() -> Stmt;
  auto declarationVar() -> Stmt;
  auto statement() -> Stmt;
  auto statementBlock() -> Stmt;
  auto statementBreak() -> Stmt;
  auto statementContinue() -> Stmt;
  auto statementIf() -> Stmt;
  auto statementNoop() -> Stmt;
  auto statementPrint() -> Stmt;
  auto statementWhile() -> Stmt;
  auto expressionStatement() -> Stmt;

  void parseInt();
  void parseFloat();
  void parseString();

  std::vector<Token>::const_iterator current;
  std::vector<Token>::const_iterator previous;
  sh::stack<Expr> expressions;
  sh::stack<Location> locations;
};
