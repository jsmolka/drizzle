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
        kPrecedenceOr,          // or
        kPrecedenceAnd,         // and
        kPrecedenceEquality,    // == !=
        kPrecedenceComparison,  // < > <= >=
        kPrecedenceTerm,        // + -
        kPrecedenceFactor,      // * / %
        kPrecedenceUnary,       // ! -
        kPrecedenceCall,        // . ()
        kPrecedencePrimary
    };

    struct Parser
    {
        Token previous;
        Token current;
    } parser;

    using ParseFunction = void(Compiler::*)(void);

    struct ParseRule
    {
        ParseFunction prefix;
        ParseFunction infix;
        Precedence precedence;
    };

    static const ParseRule& getRule(Token::Type type);

    void syntaxError(const char* error);
    void advance();
    void consume(Token::Type type, const char* error);
    void expression();
    void grouping();
    void constant();
    void unary();
    void binary();
    void literal();
    void parsePrecedence(Precedence precedence);
    void endCompiler();

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitReturn();
    void emitConstant(Value value);

    Chunk& currentChunk();

    Token* current_token;
    Tokens tokens;
    Chunk* compiling_chunk;
};
