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

    template<typename Callback>
    auto promote(const Value& op1, const Value& op2, Callback callback);

    std::tuple<Value&, Value> binaryOperands();
    void requirePrimitive(const Value& lhs, const Value& rhs, const char* error);

    void run();
    void negate();
    void not();
    void add();
    void subtract();
    void multiply();
    void divide();
    void equal();
    void notEqual();
    void modulo();

    void less();
    void lessEqual();
    void greater();
    void greaterEqual();

    const Chunk* chunk;
    const u8* ip;
    Stack<Value, 512> stack;
};
