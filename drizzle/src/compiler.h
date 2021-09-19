#pragma once

#include "chunk.h"
#include "token.h"

class Compiler
{
public:
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

    using ParseFunction = void(Compiler::*)(void);

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
    void emitConstant(Value value);

    void advance();
    void consume(Token::Type type, std::string_view error);
    void parsePrecedence(Precedence precedence);

    void binary();
    void constant();
    void expression();
    void grouping();
    void literal();
    void unary();

    Parser parser;
    Tokens::const_iterator token;
    Chunk* chunk;
};
