#pragma once

#include "chunk.h"
#include "scanner.h"

class Compiler
{
public:
    void compile(const Tokens& tokens, Chunk& chunk);

private:
    struct Parser
    {
        Token previous;
        Token current;
    } parser;

    void advance();
    void consume(Token::Type type, const char* error);
    void expression();
    void grouping();
    void unary();
    void number();
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
