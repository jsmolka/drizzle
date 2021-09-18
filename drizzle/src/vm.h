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

    template<typename Operation>
    void binary(Value& lhs, const Value& rhs, Operation op);
    std::tuple<Value&, Value> operands();
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
    void valueFalse();
    void valueNull();
    void valueTrue();

    const Chunk* chunk;
    const u8* ip;
    Stack<Value, 512> stack;
};
