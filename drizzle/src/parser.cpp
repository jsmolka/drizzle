#include "parser.h"

#include <shell/array.h>

#include "errors.h"

Stmt Parser::parse(const std::vector<Token>& tokens)
{
    current = previous = tokens.begin();

    return program();
}

const Parser::Rule& Parser::rule(Token::Type type)
{
    static constexpr auto kRulesSize = static_cast<std::size_t>(Token::Type::LastEnumValue);
    static constexpr auto kRules = shell::makeArray<Parser::Rule, kRulesSize>([](auto type) -> Parser::Rule
    {
        switch (Token::Type(type))
        {
        case Token::Type::And:          return { nullptr,           &Parser::binary, Precedence::BitAnd     };
        case Token::Type::And2:         return { nullptr,           &Parser::and_,   Precedence::And        };
        case Token::Type::Bang:         return { &Parser::unary,    nullptr,         Precedence::Term       };
        case Token::Type::BangEqual:    return { nullptr,           &Parser::binary, Precedence::Equality   };
        case Token::Type::Caret:        return { nullptr,           &Parser::binary, Precedence::BitXor     };
        case Token::Type::Equal2:       return { nullptr,           &Parser::binary, Precedence::Equality   };
        case Token::Type::False:        return { &Parser::literal,  nullptr,         Precedence::None       };
        case Token::Type::Float:        return { &Parser::constant, nullptr,         Precedence::Term       };
        case Token::Type::Greater:      return { nullptr,           &Parser::binary, Precedence::Comparison };
        case Token::Type::Greater2:     return { nullptr,           &Parser::binary, Precedence::BitShift   };
        case Token::Type::Greater3:     return { nullptr,           &Parser::binary, Precedence::BitShift   };
        case Token::Type::GreaterEqual: return { nullptr,           &Parser::binary, Precedence::Comparison };
        case Token::Type::Identifier:   return { &Parser::variable, nullptr,         Precedence::None       };
        case Token::Type::Integer:      return { &Parser::constant, nullptr,         Precedence::Term       };
        case Token::Type::Less:         return { nullptr,           &Parser::binary, Precedence::Comparison };
        case Token::Type::Less2:        return { nullptr,           &Parser::binary, Precedence::BitShift   };
        case Token::Type::LessEqual:    return { nullptr,           &Parser::binary, Precedence::Comparison };
        case Token::Type::Minus:        return { &Parser::unary,    &Parser::binary, Precedence::Term       };
        case Token::Type::Null:         return { &Parser::literal,  nullptr,         Precedence::None       };
        case Token::Type::ParenLeft:    return { &Parser::group,    nullptr,         Precedence::None       };
        case Token::Type::Percent:      return { nullptr,           &Parser::binary, Precedence::Factor     };
        case Token::Type::Pipe:         return { nullptr,           &Parser::binary, Precedence::BitOr      };
        case Token::Type::Pipe2:        return { nullptr,           &Parser::or_,    Precedence::Or         };
        case Token::Type::Plus:         return { nullptr,           &Parser::binary, Precedence::Term       };
        case Token::Type::Slash:        return { nullptr,           &Parser::binary, Precedence::Factor     };
        case Token::Type::Slash2:       return { nullptr,           &Parser::binary, Precedence::Factor     };
        case Token::Type::Star:         return { nullptr,           &Parser::binary, Precedence::Factor     };
        case Token::Type::Star2:        return { nullptr,           &Parser::binary, Precedence::Factor     };
        case Token::Type::String:       return { &Parser::constant, nullptr,         Precedence::None       };
        case Token::Type::Tilde:        return { &Parser::unary,    nullptr,         Precedence::Unary      };
        case Token::Type::True:         return { &Parser::literal,  nullptr,         Precedence::None       };
        }
        return { nullptr, nullptr, Precedence::None };
    });
    return kRules[std::size_t(type)];
}

void Parser::advance()
{
    previous = current++;
}

bool Parser::match(Token::Type type)
{
    if (current->type == type)
    {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(Token::Type type, std::string_view error)
{
    if (!match(type))
        throw SyntaxError(current->lexeme.data(), error);
}

void Parser::expectColon()      { expect(Token::Type::Colon,      "expected colon");      }
void Parser::expectDedent()     { expect(Token::Type::Dedent,     "expected dedent");     }
void Parser::expectIdentifier() { expect(Token::Type::Identifier, "expected identifier"); }
void Parser::expectIndent()     { expect(Token::Type::Indent,     "expected indent");     }
void Parser::expectNewLine()    { expect(Token::Type::NewLine,    "expected new line");   }
void Parser::expectParenLeft()  { expect(Token::Type::ParenLeft,  "expected '('");        }
void Parser::expectParenRight() { expect(Token::Type::ParenRight, "expected ')'");        }

template<typename T, typename... Args>
Expr Parser::newExpr(Args... args)
{
    return std::make_unique<Expression>(T(std::forward<Args>(args)...), previous->line);
}

Expr Parser::expression()
{
    parseExpression(Precedence::Assignment);
    return stack.popValue();
}

void Parser::parseExpression(Precedence precedence)
{
    advance();
    const auto prefix = rule(previous->type).prefix;
    if (!prefix)
        throw SyntaxError(previous->lexeme.data(), "invalid syntax");

    const auto assign = precedence <= Precedence::Assignment;
    std::invoke(prefix, this, assign);

    while (precedence <= rule(current->type).precedence)
    {
        advance();
        const auto infix = rule(previous->type).infix;
        if (!infix)
            throw SyntaxError(previous->lexeme.data(), "invalid syntax");

        std::invoke(infix, this, assign);
    }

    if (assign && match(Token::Type::Equal))
        throw SyntaxError(previous->lexeme.data(), "bad assignment");
}

void Parser::and_(bool)
{
    parseExpression(Precedence::And);

    auto rhs = stack.popValue();
    auto lhs = stack.popValue();

    stack.push(newExpr<Expression::Binary>(
        Expression::Binary::Type::And,
        std::move(lhs),
        std::move(rhs)));
}

void Parser::binary(bool)
{
    const auto type = [](Token::Type token) -> Expression::Binary::Type
    {
        switch (token)
        {
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
            SHELL_UNREACHABLE;
            return Expression::Binary::Type(-1);
        }
    };

    const auto token = previous->type;
    parseExpression(Precedence(int(rule(token).precedence) + 1));

    auto rhs = stack.popValue();
    auto lhs = stack.popValue();

    stack.push(newExpr<Expression::Binary>(
        type(token),
        std::move(lhs),
        std::move(rhs)));
}

void Parser::constant(bool)
{
    switch (previous->value.index())
    {
    case 0: stack.push(newExpr<Expression::Literal>(std::get<0>(previous->value))); break;
    case 1: stack.push(newExpr<Expression::Literal>(std::get<1>(previous->value))); break;
    case 2: stack.push(newExpr<Expression::Literal>(std::get<2>(previous->value))); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Parser::group(bool)
{
    stack.push(expression());
    expectParenRight();
}

void Parser::literal(bool)
{
    switch (previous->type)
    {
    case Token::Type::False: stack.push(newExpr<Expression::Literal>(false)); break;
    case Token::Type::Null:  stack.push(newExpr<Expression::Literal>()); break;
    case Token::Type::True:  stack.push(newExpr<Expression::Literal>(true)); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Parser::or_(bool)
{
    parseExpression(Precedence::Or);

    auto rhs = stack.popValue();
    auto lhs = stack.popValue();

    stack.push(newExpr<Expression::Binary>(
        Expression::Binary::Type::Or,
        std::move(lhs),
        std::move(rhs)));
}

void Parser::unary(bool)
{
    const auto type = [](Token::Type token) -> Expression::Unary::Type
    {
        switch (token)
        {
        case Token::Type::Bang:  return Expression::Unary::Type::Not;
        case Token::Type::Minus: return Expression::Unary::Type::Minus;
        case Token::Type::Tilde: return Expression::Unary::Type::BitwiseComplement;

        default:
            SHELL_UNREACHABLE;
            return Expression::Unary::Type(-1);
        }
    };

    const auto token = previous->type;
    parseExpression(Precedence::Unary);

    stack.push(newExpr<Expression::Unary>(type(token), stack.popValue()));
}

void Parser::variable(bool assign)
{
    const auto identifier = previous->lexeme;
    if (assign && match(Token::Type::Equal))
    {
        auto value = expression();
        stack.push(newExpr<Expression::Assign>(identifier, std::move(value)));
    }
    else
    {
        stack.push(newExpr<Expression::Variable>(identifier));
    }
}

template<typename T, typename ...Args>
Stmt Parser::newStmt(Args ...args)
{
    return std::make_unique<Statement>(T(std::forward<Args>(args)...), previous->line);
}

Stmt Parser::program()
{
    Stmts statements;

    while (!match(Token::Type::Eof))
        statements.push_back(declaration());

    return newStmt<Statement::Program>(std::move(statements));
}

Stmt Parser::declaration()
{
    if (match(Token::Type::Var))
        return declarationVar();

    return statement();
}

Stmt Parser::declarationVar()
{
    expectIdentifier();
    const auto identifier = previous->lexeme;

    Expr initializer;
    if (match(Token::Type::Equal))
        initializer = expression();
    else
        initializer = newExpr<Expression::Literal>();

    expectNewLine();

    return newStmt<Statement::Var>(identifier, std::move(initializer));
}

Stmt Parser::statement()
{
    if (match(Token::Type::Block))
        return statementBlock();
    if (match(Token::Type::Break))
        return statementBreak();
    if (match(Token::Type::Continue))
        return statementContinue();
    if (match(Token::Type::If))
        return statementIf();
    if (match(Token::Type::Noop))
        return statementNoop();
    if (match(Token::Type::Print))
        return statementPrint();
    if (match(Token::Type::While))
        return statementWhile();

    return expressionStatement();
}

Stmt Parser::statementBlock()
{
    std::string_view identifier;
    if (match(Token::Type::Identifier))
        identifier = previous->lexeme;

    expectColon();
    expectNewLine();
    expectIndent();

    Stmts statements;
    while (current->type != Token::Type::Dedent)
        statements.push_back(declaration());

    expectDedent();

    return newStmt<Statement::Block>(identifier, std::move(statements));
}

Stmt Parser::statementBreak()
{
    std::string_view identifier;
    if (match(Token::Type::Identifier))
        identifier = previous->lexeme;

    expectNewLine();
    return newStmt<Statement::Break>(identifier);
}

Stmt Parser::statementContinue()
{
    expectNewLine();
    return newStmt<Statement::Continue>();
}

Stmt Parser::statementIf()
{
    const auto branch = [this]()
    {
        auto condition = expression();
        expectColon();
        expectNewLine();
        expectIndent();

        Stmts statements;
        while (current->type != Token::Type::Dedent)
            statements.push_back(declaration());

        expectDedent();

        return Statement::If::Branch(std::move(condition), std::move(statements));
    };

    auto if_ = branch();
    std::vector<Statement::If::Branch> elifs;
    while (match(Token::Type::Elif))
        elifs.push_back(branch());

    Stmts else_;
    if (match(Token::Type::Else))
    {
        expectColon();
        expectNewLine();
        expectIndent();

        while (current->type != Token::Type::Dedent)
            else_.push_back(declaration());

        expectDedent();
    }

    return newStmt<Statement::If>(std::move(if_), std::move(elifs), std::move(else_));
}

Stmt Parser::statementNoop()
{
    expectNewLine();
    return newStmt<Statement::Noop>();
}

Stmt Parser::statementPrint()
{
    auto expr = expression();
    expectNewLine();
    return newStmt<Statement::Print>(std::move(expr));
}

Stmt Parser::statementWhile()
{
    auto condition = expression();
    expectColon();
    expectNewLine();
    expectIndent();

    Stmts statements;
    while (current->type != Token::Type::Dedent)
        statements.push_back(declaration());

    expectDedent();

    return newStmt<Statement::While>(std::move(condition), std::move(statements));
}

Stmt Parser::expressionStatement()
{
    auto expr = expression();
    expectNewLine();
    return newStmt<Statement::ExpressionStatement>(std::move(expr));
}
