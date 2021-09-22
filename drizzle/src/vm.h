#pragma once

#include "chunk.h"
#include "interning.h"
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
    void raiseTypeError(std::string_view operation, const DzValue& value);
    void raiseTypeError(std::string_view operation, const DzValue& lhs, const DzValue& rhs);

    template<typename Operation>
    void bitwiseBinary(DzValue& lhs, const DzValue& rhs, Operation operation);
    template<typename Operation>
    void primitiveBinary(DzValue& lhs, const DzValue& rhs, Operation operation);
    std::tuple<DzValue&, DzValue> operands();
    std::tuple<DzValue&, DzValue> bitwiseOperands(std::string_view operation);
    std::tuple<DzValue&, DzValue> primitiveOperands(std::string_view operation);

    void add();
    void assertion();
    void bitAnd();
    void bitAsr();
    void bitComplement();
    void bitLsl();
    void bitLsr();
    void bitOr();
    void bitXor();
    void constant();
    void constantExt();
    void defineGlobalVar();
    void defineGlobalVarExt();
    void discard();
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
    void not_();
    void notEqual();
    void power();
    void print();
    void subtract();
    void valueFalse();
    void valueNull();
    void valueTrue();

    Chunk chunk;
    const u8* ip;
    Stack<DzValue, 512> stack;
    Interning interning;
    DzTable<DzValue> globals;
};
