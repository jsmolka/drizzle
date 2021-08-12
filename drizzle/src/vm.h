#pragma once

#include <string_view>

#include "chunk.h"
#include "stack.h"

class Vm
{
public:
    void interpret(std::string_view source);

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
