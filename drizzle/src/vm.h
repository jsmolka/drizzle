#pragma once

#include "chunk.h"
#include "stack.h"
#include "scanner.h"

class Vm
{
public:
    void interpret(const Tokens& tokens);

private:
    template<typename Integral>
    Integral read();

    void run();
    void negate();
    void add();
    void subtract();
    void multiply();
    void divide();

    const Chunk* chunk;
    const u8* ip;
    Stack<Value, 512> stack;
};
