#include "parser.h"

#include <sh/array.h>
#include <sh/parse.h>
#include <sh/ranges.h>
#include <sh/utility.h>

#include "error.h"

auto Parser::parse(const std::vector<Token>& tokens) -> Stmt {
  current = previous = tokens.begin();
  return program();
}

auto Parser::rule(Token::Type type) -> const Rule& {
  static constexpr auto kRulesSize = std::size_t(Token::Type::LastEnumValue);
  static constexpr auto kRules = sh::make_array<Rule, kRulesSize>([]<std::size_t kType>() -> Rule {
    switch (Token::Type(kType)) {
      case Token::Type::And:          return {nullptr,           &Parser::binary,    Precedence::BitAnd    };
      case Token::Type::And2:         return {nullptr,           &Parser::and_,      Precedence::And       };
      case Token::Type::Bang:         return {&Parser::unary,    nullptr,            Precedence::Term      };
      case Token::Type::BangEqual:    return {nullptr,           &Parser::binary,    Precedence::Equality  };
      case Token::Type::BracketLeft:  return {&Parser::list,     &Parser::subscript, Precedence::Call      };
      case Token::Type::BraceLeft:    return {&Parser::map,      nullptr,            Precedence::None      };
      case Token::Type::Caret:        return {nullptr,           &Parser::binary,    Precedence::BitXor    };
      case Token::Type::Dot:          return {nullptr,           &Parser::dot,       Precedence::Call      };
      case Token::Type::Dot2:         return {nullptr,           &Parser::range,     Precedence::Range     };
      case Token::Type::Equal2:       return {nullptr,           &Parser::binary,    Precedence::Equality  };
      case Token::Type::False:        return {&Parser::literal,  nullptr,            Precedence::None      };
      case Token::Type::Float:        return {&Parser::constant, nullptr,            Precedence::None      };
      case Token::Type::Greater:      return {nullptr,           &Parser::binary,    Precedence::Comparison};
      case Token::Type::Greater2:     return {nullptr,           &Parser::binary,    Precedence::BitShift  };
      case Token::Type::Greater3:     return {nullptr,           &Parser::binary,    Precedence::BitShift  };
      case Token::Type::GreaterEqual: return {nullptr,           &Parser::binary,    Precedence::Comparison};
      case Token::Type::Identifier:   return {&Parser::variable, nullptr,            Precedence::None      };
      case Token::Type::In:           return {nullptr,           &Parser::in,        Precedence::In        };
      case Token::Type::Integer:      return {&Parser::constant, nullptr,            Precedence::None      };
      case Token::Type::Less:         return {nullptr,           &Parser::binary,    Precedence::Comparison};
      case Token::Type::Less2:        return {nullptr,           &Parser::binary,    Precedence::BitShift  };
      case Token::Type::LessEqual:    return {nullptr,           &Parser::binary,    Precedence::Comparison};
      case Token::Type::Minus:        return {&Parser::unary,    &Parser::binary,    Precedence::Term      };
      case Token::Type::Null:         return {&Parser::literal,  nullptr,            Precedence::None      };
      case Token::Type::ParenLeft:    return {&Parser::group,    &Parser::call,      Precedence::Call      };
      case Token::Type::Percent:      return {nullptr,           &Parser::binary,    Precedence::Factor    };
      case Token::Type::Pipe:         return {nullptr,           &Parser::binary,    Precedence::BitOr     };
      case Token::Type::Pipe2:        return {nullptr,           &Parser::or_,       Precedence::Or        };
      case Token::Type::Plus:         return {nullptr,           &Parser::binary,    Precedence::Term      };
      case Token::Type::Slash:        return {nullptr,           &Parser::binary,    Precedence::Factor    };
      case Token::Type::Slash2:       return {nullptr,           &Parser::binary,    Precedence::Factor    };
      case Token::Type::Star:         return {nullptr,           &Parser::binary,    Precedence::Factor    };
      case Token::Type::Star2:        return {nullptr,           &Parser::binary,    Precedence::Factor    };
      case Token::Type::String:       return {&Parser::constant, nullptr,            Precedence::None      };
      case Token::Type::This:         return {&Parser::this_,    nullptr,            Precedence::None      };
      case Token::Type::Tilde:        return {&Parser::unary,    nullptr,            Precedence::Unary     };
      case Token::Type::True:         return {&Parser::literal,  nullptr,            Precedence::None      };
    }
    return {nullptr, nullptr, Precedence::None};
  });
  return kRules[std::size_t(type)];
}

void Parser::advance() {
  previous = current++;
}

auto Parser::match(Token::Type type) -> bool {
  const auto matched = current->type == type;
  if (matched) {
    advance();
  }
  return matched;
}

void Parser::expect(Token::Type type, std::string_view error) {
  if (!match(type)) {
    throw SyntaxError(current->location, error);
  }
}

void Parser::expectBraceRight()   { expect(Token::Type::BraceRight,   "expected '}'");                 }
void Parser::expectBracketRight() { expect(Token::Type::BracketRight, "expected ']'");                 }
void Parser::expectCase()         { expect(Token::Type::Case,         "expected 'case'");              }
void Parser::expectColon()        { expect(Token::Type::Colon,        "expected colon");               }
void Parser::expectDedent()       { expect(Token::Type::Dedent,       "expected dedent");              }
void Parser::expectDef()          { expect(Token::Type::Def,          "expected function definition"); }
void Parser::expectIn()           { expect(Token::Type::In,           "expected 'in'");                }
void Parser::expectIndent()       { expect(Token::Type::Indent,       "expected indent");              }
void Parser::expectNewLine()      { expect(Token::Type::NewLine,      "expected new line");            }
void Parser::expectParenLeft()    { expect(Token::Type::ParenLeft,    "expected '('");                 }
void Parser::expectParenRight()   { expect(Token::Type::ParenRight,   "expected ')'");                 }

auto Parser::expectIdentifier() -> Identifier {
  expect(Token::Type::Identifier, "expected identifier");
  return identifier();
}

auto Parser::identifier() const -> Identifier {
  assert(previous->type == Token::Type::Identifier || previous->type == Token::Type::This);
  return Identifier(previous->lexeme, previous->location);
}

auto Parser::skipWhitespace() -> int {
  int indent = 0;
  while (true) {
    switch (current->type) {
      case Token::Type::Indent:
        advance();
        indent++;
        break;
      case Token::Type::Dedent:
        advance();
        indent--;
        break;
      case Token::Type::NewLine:
        advance();
        break;
      default:
        return indent;
    }
  }
}

auto Parser::arguments() -> Exprs {
  Exprs arguments;
  if (current->type != Token::Type::ParenRight) {
    do {
      arguments.push_back(expression());
    } while (match(Token::Type::Comma));
  }
  expectParenRight();
  return arguments;
}

template<typename T>
auto Parser::newExpr(T expression) -> Expr {
  return std::make_unique<Expression>(std::move(expression), previous->location);
}

auto Parser::expression() -> Expr {
  parseExpression(Precedence::Assignment);
  return expressions.pop_value();
}

void Parser::parseExpression(Precedence precedence) {
  advance();
  const auto prefix = rule(previous->type).prefix;
  if (!prefix) {
    throw SyntaxError(previous->location, "invalid syntax");
  }

  const auto assign = precedence <= Precedence::Assignment;
  std::invoke(prefix, this, assign);

  while (precedence <= rule(current->type).precedence) {
    advance();
    const auto infix = rule(previous->type).infix;
    if (!infix) {
      throw SyntaxError(previous->location, "invalid syntax");
    }
    std::invoke(infix, this, assign);
  }

  if (assign && match(Token::Type::Equal)) {
    throw SyntaxError(previous->location, "bad assignment");
  }
}

void Parser::and_(bool) {
  parseExpression(Precedence::And);
  auto rhs = expressions.pop_value();
  auto lhs = expressions.pop_value();

  expressions.push(newExpr(Expression::Binary{
    .type = Expression::Binary::Type::And,
    .left = std::move(lhs),
    .right = std::move(rhs)
  }));
}

void Parser::binary(bool) {
  static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

  auto type = [](Token::Type token) {
    switch (token) {
      case Token::Type::And:          return Expression::Binary::Type::BitwiseAnd;
      case Token::Type::BangEqual:    return Expression::Binary::Type::NotEqual;
      case Token::Type::Caret:        return Expression::Binary::Type::BitwiseXor;
      case Token::Type::Equal2:       return Expression::Binary::Type::Equal;
      case Token::Type::Greater:      return Expression::Binary::Type::Greater;
      case Token::Type::Greater2:     return Expression::Binary::Type::BitwiseAsr;
      case Token::Type::Greater3:     return Expression::Binary::Type::BitwiseLsr;
      case Token::Type::GreaterEqual: return Expression::Binary::Type::GreaterEqual;
      case Token::Type::Less:         return Expression::Binary::Type::Less;
      case Token::Type::Less2:        return Expression::Binary::Type::BitwiseLsl;
      case Token::Type::LessEqual:    return Expression::Binary::Type::LessEqual;
      case Token::Type::Minus:        return Expression::Binary::Type::Subtraction;
      case Token::Type::Plus:         return Expression::Binary::Type::Addition;
      case Token::Type::Percent:      return Expression::Binary::Type::Modulo;
      case Token::Type::Pipe:         return Expression::Binary::Type::BitwiseOr;
      case Token::Type::Slash:        return Expression::Binary::Type::Division;
      case Token::Type::Slash2:       return Expression::Binary::Type::IntegerDivision;
      case Token::Type::Star:         return Expression::Binary::Type::Multiplication;
      case Token::Type::Star2:        return Expression::Binary::Type::Power;
      default:
        SH_UNREACHABLE;
        return Expression::Binary::Type(-1);
    }
  };

  const auto token = previous->type;
  parseExpression(Precedence(int(rule(token).precedence) + 1));
  auto rhs = expressions.pop_value();
  auto lhs = expressions.pop_value();

  expressions.push(newExpr(Expression::Binary{
    .type = type(token),
    .left = std::move(lhs),
    .right = std::move(rhs)
  }));
}

void Parser::call(bool) {
  auto arguments = this->arguments();
  expressions.push(newExpr(Expression::Call{
    .callee = expressions.pop_value(),
    .arguments = std::move(arguments)
  }));
}

void Parser::constant(bool) {
  switch (previous->type) {
    case Token::Type::Integer: parseInt(); break;
    case Token::Type::Float:   parseFloat(); break;
    case Token::Type::String:  parseString(); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void Parser::dot(bool assign) {
  const auto identifier = expectIdentifier();

  auto self = expressions.pop_value();
  if (assign && match(Token::Type::Equal)) {
    expressions.push(newExpr(Expression::Set{
      .identifier = identifier,
      .self = std::move(self),
      .value = expression()
    }));
  } else if (match(Token::Type::ParenLeft)) {
    auto arguments = this->arguments();
    expressions.push(newExpr(Expression::Invoke{
      .identifier = identifier,
      .self = std::move(self),
      .arguments = std::move(arguments)
    }));
  } else {
    expressions.push(newExpr(Expression::Get{
      .identifier = identifier,
      .self = std::move(self)
    }));
  }
}

void Parser::group(bool) {
  expressions.push(expression());
  expectParenRight();
}

void Parser::in(bool) {
  expressions.push(newExpr(Expression::In{
    .self = expression(),
    .expression = expressions.pop_value()
  }));
}

void Parser::list(bool) {
  auto indent = skipWhitespace();

  Exprs values;
  if (current->type != Token::Type::BracketRight) {
    do {
      indent += skipWhitespace();
      values.push_back(expression());
      indent += skipWhitespace();
    } while (match(Token::Type::Comma));
  }

  if (indent != 0) {
    throw SyntaxError(current->location, "bad list format");
  }

  expectBracketRight();
  expressions.push(newExpr(Expression::List{
    .values = std::move(values)
  }));
}

void Parser::literal(bool) {
  switch (previous->type) {
    case Token::Type::True:  expressions.push(newExpr(Expression::Literal{true})); break;
    case Token::Type::False: expressions.push(newExpr(Expression::Literal{false})); break;
    case Token::Type::Null:  expressions.push(newExpr(Expression::Literal{})); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void Parser::map(bool) {
  auto indent = skipWhitespace();

  std::vector<Expression::Map::Pair> pairs;
  if (current->type != Token::Type::BraceRight) {
    do {
      indent += skipWhitespace();
      auto key = expression();
      expectColon();
      auto value = expression();
      pairs.push_back(Expression::Map::Pair{
        .key = std::move(key),
        .value = std::move(value)
      });
      indent += skipWhitespace();
    } while (match(Token::Type::Comma));
  }

  if (indent != 0) {
    throw SyntaxError(current->location, "bad map format");
  }

  expectBraceRight();
  expressions.push(newExpr(Expression::Map{
    .pairs = std::move(pairs)
  }));
}

void Parser::or_(bool) {
  parseExpression(Precedence::Or);
  auto rhs = expressions.pop_value();
  auto lhs = expressions.pop_value();

  expressions.push(newExpr(Expression::Binary{
    .type = Expression::Binary::Type::Or,
    .left = std::move(lhs),
    .right = std::move(rhs)
  }));
}

void Parser::range(bool) {
  expressions.push(newExpr(Expression::Range{
    .start = expressions.pop_value(),
    .stop = expression()
  }));
}

void Parser::subscript(bool assign) {
  auto self = expressions.pop_value();
  auto expr = expression();
  expectBracketRight();

  if (assign && match(Token::Type::Equal)) {
    expressions.push(newExpr(Expression::SubscriptSet{
      .self = std::move(self),
      .expression = std::move(expr),
      .value = expression()
    }));
  } else {
    expressions.push(newExpr(Expression::SubscriptGet{
      .self = std::move(self),
      .expression = std::move(expr)
    }));
  }
}

void Parser::this_(bool) {
  variable(false);
}

void Parser::unary(bool) {
  static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);

  auto type = [](Token::Type token) {
    switch (token) {
      case Token::Type::Bang:  return Expression::Unary::Type::Not;
      case Token::Type::Minus: return Expression::Unary::Type::Minus;
      case Token::Type::Tilde: return Expression::Unary::Type::BitwiseComplement;
      default:
        SH_UNREACHABLE;
        return Expression::Unary::Type(-1);
    }
  };

  const auto token = previous->type;
  parseExpression(Precedence::Unary);

  expressions.push(newExpr(Expression::Unary{
    .type = type(token),
    .expression = expressions.pop_value()
  }));
}

void Parser::variable(bool assign) {
  const auto identifier = this->identifier();
  if (assign && match(Token::Type::Equal)) {
    auto value = expression();
    expressions.push(newExpr(Expression::Assign{
      .identifier = identifier,
      .value = std::move(value)
    }));
  } else {
    expressions.push(newExpr(Expression::Variable{identifier}));
  }
}

auto Parser::block() -> Stmts {
  expectColon();
  expectNewLine();
  expectIndent();

  Stmts statements;
  while (current->type != Token::Type::Dedent) {
    statements.push_back(declaration());
  }

  expectDedent();
  return statements;
}

template<typename T>
auto Parser::newStmt(T statement) -> Stmt {
  return std::make_unique<Statement>(std::move(statement), locations.pop_value());
}

void Parser::pushLocation() {
  locations.push(previous->location);
}

auto Parser::program() -> Stmt {
  pushLocation();
  Stmts statements;
  while (!match(Token::Type::Eof)) {
    statements.push_back(declaration());
  }
  return newStmt(Statement::Program{std::move(statements)});
}

auto Parser::declaration() -> Stmt {
  if (match(Token::Type::Class)) {
    return declarationClass();
  } else if (match(Token::Type::Def)) {
    return declarationDef();
  } else if (match(Token::Type::Var)) {
    return declarationVar();
  }
  return statement();
}

auto Parser::declarationClass() -> Stmt {
  pushLocation();
  const auto identifier = expectIdentifier();

  expectColon();
  expectNewLine();
  expectIndent();

  Stmts methods;
  if (match(Token::Type::Noop)) {
    expectNewLine();
  } else {
    do {
      expectDef();
      methods.push_back(declarationDef());
    }
    while (current->type != Token::Type::Dedent);
  }

  expectDedent();
  return newStmt(Statement::Class{
    .identifier = identifier,
    .methods = std::move(methods)
  });
}

auto Parser::declarationDef() -> Stmt {
  pushLocation();
  const auto identifier = expectIdentifier();

  expectParenLeft();
  std::vector<Identifier> parameters;
  if (current->type == Token::Type::Identifier) {
    do {
      parameters.push_back(expectIdentifier());
    } while (match(Token::Type::Comma));
  }
  expectParenRight();

  auto statements = block();
  return newStmt(Statement::Def{
    .identifier = identifier,
    .parameters = std::move(parameters),
    .statements = std::move(statements)
  });
}

auto Parser::declarationVar() -> Stmt {
  pushLocation();
  const auto identifier = expectIdentifier();

  Expr initializer;
  if (match(Token::Type::Equal)) {
    initializer = expression();
  } else {
    initializer = newExpr(Expression::Literal{});
  }
  expectNewLine();
  return newStmt(Statement::Var{
    .identifier = identifier,
    .initializer = std::move(initializer)
  });
}

auto Parser::statement() -> Stmt {
  if (match(Token::Type::Block)) {
    return statementBlock();
  } else if (match(Token::Type::Break)) {
    return statementBreak();
  } else if (match(Token::Type::Continue)) {
    return statementContinue();
  } else if (match(Token::Type::If)) {
    return statementIf();
  } else if (match(Token::Type::For)) {
    return statementFor();
  } else if (match(Token::Type::Noop)) {
    return statementNoop();
  } else if (match(Token::Type::Return)) {
    return statementReturn();
  } else if (match(Token::Type::Switch)) {
    return statementSwitch();
  } else if (match(Token::Type::While)) {
    return statementWhile();
  }
  return expressionStatement();
}

auto Parser::statementBlock() -> Stmt {
  pushLocation();
  std::optional<Identifier> identifier;
  if (match(Token::Type::Identifier)) {
    identifier = this->identifier();
  }

  auto statements = block();
  return newStmt(Statement::Block{
    .identifier = identifier,
    .statements = std::move(statements)
  });
}

auto Parser::statementBreak() -> Stmt {
  pushLocation();
  std::optional<Identifier> identifier;
  if (match(Token::Type::Identifier)) {
    identifier = this->identifier();
  }
  expectNewLine();
  return newStmt(Statement::Break{identifier});
}

auto Parser::statementContinue() -> Stmt {
  pushLocation();
  expectNewLine();
  return newStmt(Statement::Continue{});
}

auto Parser::statementFor() -> Stmt {
  pushLocation();
  const auto iterator = expectIdentifier();
  expectIn();
  auto iteree = expression();
  auto statements = block();
  return newStmt(Statement::For{
    .iterator = iterator,
    .iteree = std::move(iteree),
    .statements = std::move(statements)
  });
}

auto Parser::statementIf() -> Stmt {
  pushLocation();

  std::vector<Statement::If::Branch> branches;
  branches.emplace_back(Statement::If::Branch{expression(), block()});
  while (match(Token::Type::Elif)) {
    branches.push_back(Statement::If::Branch{expression(), block()});
  }

  std::optional<Stmts> else_;
  if (match(Token::Type::Else)) {
    else_ = block();
  }
  return newStmt(Statement::If{
    .branches = std::move(branches),
    .else_ = std::move(else_)
  });
}

auto Parser::statementNoop() -> Stmt {
  pushLocation();
  expectNewLine();
  return newStmt(Statement::Noop{});
}

auto Parser::statementReturn() -> Stmt {
  pushLocation();
  std::optional<Expr> expr;
  if (current->type != Token::Type::NewLine) {
    expr = expression();
  }
  expectNewLine();
  return newStmt(Statement::Return{std::move(expr)});
}

auto Parser::statementSwitch() -> Stmt {
  pushLocation();

  auto value = expression();
  expectColon();
  expectNewLine();
  expectIndent();

  std::vector<Statement::Switch::Case> cases;
  expectCase();
  do {
    Exprs values;
    do {
      values.push_back(expression());
      expectColon();
      expectNewLine();
    } while (match(Token::Type::Case));

    expectIndent();

    Stmts statements;
    while (current->type != Token::Type::Dedent) {
      statements.push_back(declaration());
    }

    expectDedent();
    cases.push_back(Statement::Switch::Case{
      .values = std::move(values),
      .statements = std::move(statements)
    });
  } while (match(Token::Type::Case));

  std::optional<Stmts> default_;
  if (match(Token::Type::Default)) {
    default_ = block();
  }

  expectDedent();
  return newStmt(Statement::Switch{
    .value = std::move(value),
    .cases = std::move(cases),
    .default_ = std::move(default_)
  });
}

auto Parser::statementWhile() -> Stmt {
  pushLocation();
  auto condition = expression();
  expectColon();
  expectNewLine();
  expectIndent();

  Stmts statements;
  while (current->type != Token::Type::Dedent) {
    statements.push_back(declaration());
  }
  expectDedent();
  return newStmt(Statement::While{
    .condition = std::move(condition),
    .statements = std::move(statements)
  });
}

auto Parser::expressionStatement() -> Stmt {
  pushLocation();
  auto expr = expression();
  expectNewLine();
  return newStmt(Statement::ExpressionStatement{std::move(expr)});
}

void Parser::parseInt() {
  if (const auto value = sh::parse<std::make_unsigned_t<dzint>>(previous->lexeme)) {
    expressions.push(newExpr<Expression::Literal>(static_cast<dzint>(*value)));
  } else {
    throw SyntaxError(previous->location, "cannot parse int");
  }
}

void Parser::parseFloat() {
  if (const auto value = sh::parse<double>(previous->lexeme)) {
    expressions.push(newExpr<Expression::Literal>(*value));
  } else {
    throw SyntaxError(previous->location, "cannot parse float");
  }
}

void Parser::parseString() {
  auto lexeme = previous->lexeme;
  auto quotes = lexeme.starts_with(R"(""")") ? 3 : 1;
  lexeme.remove_prefix(quotes);
  lexeme.remove_suffix(quotes);

  std::string string;
  string.reserve(lexeme.size());
  if (quotes == 1) {
    auto escape = false;
    for (const auto& c : lexeme) {
      if (escape) {
        switch (c) {
          case '\\': string.push_back('\\'); break;
          case '\"': string.push_back('\"'); break;
          case 'n':  string.push_back('\n'); break;
          case 'r':  string.push_back('\r'); break;
          case 't':  string.push_back('\t'); break;
          default:
            SH_UNREACHABLE;
            break;
        }
        escape = false;
      } else if (c == '\\') {
        escape = true;
      } else {
        string.push_back(c);
      }
    }
  } else {
    string = lexeme;
  }
  expressions.push(newExpr<Expression::Literal>(std::move(string)));
}
