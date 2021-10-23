#pragma once

#include "statement.h"
#include "token.h"

class Parser
{
public:
    Stmt parse(const std::vector<Token>& tokens);

private:
    void advance();
    bool match(Token::Type type);
    void expect(Token::Type type, std::string_view error);
    void expectColon();
    void expectDedent();
    void expectIdentifier();
    void expectIndent();
    void expectNewLine();
    void expectParenLeft();
    void expectParenRight();

    Expr expression();

    Stmt program();
    Stmt declaration();
    Stmt statement();
    Stmt expressionStatement();

    std::vector<Token>::const_iterator current;
    std::vector<Token>::const_iterator previous;
};
