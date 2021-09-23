#pragma once

#include "chunk.h"
#include "interning.h"
#include "token.h"

class Compiler
{
public:
    Compiler(Interning& interning);

    void compile(const Tokens& tokens, Chunk& chunk);

private:
    enum Precedence
    {
        kPrecedenceNone,
        kPrecedenceAssignment,  // =
        kPrecedenceOr,          // ||
        kPrecedenceAnd,         // &&
        kPrecedenceBitOr,       // |
        kPrecedenceBitXor,      // ^
        kPrecedenceBitAnd,      // &
        kPrecedenceEquality,    // == !=
        kPrecedenceComparison,  // < > <= >=
        kPrecedenceBitShift,    // << >> >>>
        kPrecedenceTerm,        // + -
        kPrecedenceFactor,      // * / // %
        kPrecedenceUnary,       // ! -
        kPrecedenceCall,        // . ()
        kPrecedencePrimary
    };

    struct Parser
    {
        Token current;
        Token previous;
    };

    using ParseFunction = void(Compiler::*)(bool);

    struct ParseRule
    {
        ParseFunction prefix;
        ParseFunction infix;
        Precedence precedence;
    };

    static const ParseRule& rule(Token::Type type);

    template<typename Error, typename... Args>
    void raise(std::string_view message, Args&&... args);

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitValue(DzValue value, Opcode small, Opcode large);

    void advance();
    bool check(Token::Type type) const;
    bool match(Token::Type type);
    void consume(Token::Type type, std::string_view error);
    void consumeNewLine();
    void parsePrecedence(Precedence precedence);

    void binary(bool);
    void constant(bool);
    void declaration();
    void declarationVar();
    void expression();
    void grouping(bool);
    void literal(bool);
    void statement();
    void statementAssert();
    void statementExpression();
    void statementPrint();
    void unary(bool);
    void variable(bool can_assign);

    Parser parser;
    Tokens::const_iterator token;
    Chunk* chunk;
    Interning& interning;
};
