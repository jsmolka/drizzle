#pragma once

#include <sh/stack.h>

#include "statement.h"
#include "token.h"

class Parser {
public:
  auto parse(const std::vector<Token>& tokens) -> Stmt;

private:
  using Iterator = std::vector<Token>::const_iterator;

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
    Call,
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
  void expectBracketRight();
  void expectColon();
  void expectDedent();
  void expectDef();
  void expectIndent();
  void expectNewLine();
  void expectParenLeft();
  void expectParenRight();
  auto expectIdentifier() -> Identifier;
  auto identifier() const -> Identifier;

  template<typename T>
  auto newExpr(T expression) -> Expr;
  auto expression() -> Expr;
  void parseExpression(Precedence precedence);
  void and_(bool);
  void binary(bool);
  void list(bool);
  void call(bool);
  void constant(bool);
  void dot(bool assign);
  void group(bool);
  void literal(bool);
  void or_(bool);
  void this_(bool);
  void unary(bool);
  void variable(bool assign);

  template<typename T>
  auto newStmt(T statement) -> Stmt;
  void pushLocation();
  auto program() -> Stmt;
  auto declaration() -> Stmt;
  auto declarationClass() -> Stmt;
  auto declarationDef() -> Stmt;
  auto declarationVar() -> Stmt;
  auto statement() -> Stmt;
  auto statementBlock() -> Stmt;
  auto statementBreak() -> Stmt;
  auto statementContinue() -> Stmt;
  auto statementIf() -> Stmt;
  auto statementNoop() -> Stmt;
  auto statementReturn() -> Stmt;
  auto statementWhile() -> Stmt;
  auto expressionStatement() -> Stmt;

  void parseInt();
  void parseFloat();
  void parseString();

  Iterator current;
  Iterator previous;
  sh::stack<Expr> expressions;
  sh::stack<Location> locations;
};
