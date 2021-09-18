#pragma once

#include "chunk.h"
#include "scanner.h"

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
        kPrecedenceEquality,    // == !=
        kPrecedenceComparison,  // < > <= >=
        kPrecedenceTerm,        // + -
        kPrecedenceFactor,      // * / // %
        kPrecedenceUnary,       // ! -
        kPrecedenceCall,        // . ()
        kPrecedencePrimary
    };

    struct Parser
    {
        Token previous;
        Token current;
    };

    using ParseFunction = void(Compiler::*)(void);

    struct ParseRule
    {
        ParseFunction prefix;
        ParseFunction infix;
        Precedence precedence;
    };

    static const ParseRule& rule(Token::Type type);

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitConstant(Value value);

    template<typename Error>
    void raise(const std::string& message);

    void advance();
    void consume(Token::Type type, const char* error);
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
