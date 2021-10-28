#pragma once

#include <shell/stack.h>

#include "statement.h"
#include "token.h"

class Parser
{
public:
    Stmt parse(const std::vector<Token>& tokens);

private:
    enum class Precedence
    {
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

    struct Rule
    {
        using Parselet = void(Parser::*)(bool);

        Parselet prefix;
        Parselet infix;
        Precedence precedence;
    };

    static const Parser::Rule& rule(Token::Type type);

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

    template<typename T, typename... Args>
    Expr newExpr(Args... args);
    Expr expression();
    void parseExpression(Precedence precedence);
    void and_(bool);
    void binary(bool);
    void constant(bool);
    void group(bool);
    void literal(bool);
    void or_(bool);
    void unary(bool);
    void variable(bool assign);

    template<typename T, typename... Args>
    Stmt newStmt(Args... args);
    Stmt program();
    Stmt declaration();
    Stmt declarationVar();
    Stmt statement();
    Stmt statementBlock();
    Stmt statementNoop();
    Stmt statementPrint();
    Stmt expressionStatement();

    std::vector<Token>::const_iterator current;
    std::vector<Token>::const_iterator previous;
    shell::Stack<Expr, 32> stack;
};
