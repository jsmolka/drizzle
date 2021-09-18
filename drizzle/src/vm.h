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

    void add();
    void constant();
    void constantExt();
    void divide();
    void equal();
    void greater();
    void greaterEqual();
    void less();
    void lessEqual();
    void modulo();
    void multiply();
    void negate();
    void not();
    void notEqual();
    void subtract();
    void valueTrue();
    void valueFalse();
    void valueNull();

    const Chunk* chunk;
    const u8* ip;
    Stack<Value, 512> stack;
};
