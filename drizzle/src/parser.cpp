#include "parser.h"

#include "errors.h"

Stmt Parser::parse(const std::vector<Token>& tokens)
{
    current = previous = tokens.begin();

    return program();
}

void Parser::advance()
{
    current = previous++;
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
        throw SyntaxError(current->lexeme, error);
}

void Parser::expectColon()
{
    expect(Token::Type::Colon, "expected colon");
}

void Parser::expectDedent()
{
    expect(Token::Type::Dedent, "expected dedent");
}

void Parser::expectIdentifier()
{
    expect(Token::Type::Identifier, "expected identifier");
}

void Parser::expectIndent()
{
    expect(Token::Type::Indent, "expected indent");
}

void Parser::expectNewLine()
{
    expect(Token::Type::NewLine, "expected new line");
}

void Parser::expectParenLeft()
{
    expect(Token::Type::ParenLeft, "expected '('");
}

void Parser::expectParenRight()
{
    expect(Token::Type::ParenRight, "expected ')'");
}

Expr Parser::expression()
{
    return Expr();
}

Stmt Parser::program()
{
    std::vector<Stmt> statements;

    while (!match(Token::Type::Eof))
        statements.push_back(declaration());

    return newStmt<Statement::Program>(std::move(statements));
}

Stmt Parser::declaration()
{
    return statement();
}

Stmt Parser::statement()
{
    return expressionStatement();
}

Stmt Parser::expressionStatement()
{
    auto expr = expression();

    return newStmt<Statement::ExpressionStatement>(std::move(expr));
}
