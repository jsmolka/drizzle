#pragma once

#include <functional>
#include <shell/punning.h>

#include "chunk.h"
#include "stack.h"

class Vm
{
public:
    void interpret(const Chunk& chunk);

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

inline Vm vm;
