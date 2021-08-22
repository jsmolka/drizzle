#pragma once

#include "chunk.h"
#include "stack.h"
#include "token.h"

class Vm
{
public:
    void interpret(const Tokens& tokens);

private:
    template<typename Integral>
    Integral read();

    void run();

    template<typename Operator>
    void binaryOperation();

    const Chunk* chunk;
    const u8* ip;
    Stack<Value, 512> stack;
};
