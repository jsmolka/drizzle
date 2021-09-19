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

    template<typename Error, typename... Args>
    void raise(std::string_view message, Args&&... args);

    template<typename Operation>
    void primitiveBinary(Value& lhs, const Value& rhs, Operation op);
    std::tuple<Value&, Value> operands();
    std::tuple<Value&, Value> primitiveOperands(std::string_view operation);

    void add();
    void constant();
    void constantExt();
    void divide();
    void divideInt();
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

    Chunk chunk;
    const u8* ip;
    Stack<Value, 512> stack;
};
