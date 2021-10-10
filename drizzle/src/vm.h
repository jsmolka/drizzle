#pragma once

#include <shell/stack.h>

#include "chunk.h"
#include "interning.h"
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

    template<template<typename T> typename Promote = promote_t, typename Handler>
    void unary(std::string_view operation, Handler callback);
    template<template<typename T, typename U> typename Promote = promote_t, typename Handler>
    void binary(std::string_view operation, Handler callback);

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
    void divide();
    void divideInt();
    void equal();
    void greater();
    void greaterEqual();
    void jump();
    void jumpFalse();
    void jumpFalsePop();
    void jumpTrue();
    void less();
    void lessEqual();
    void loadVariable();
    void loadVariableExt();
    void modulo();
    void multiply();
    void negate();
    void not_();
    void notEqual();
    void pop();
    void popMultiple();
    void popMultipleExt();
    void power();
    void print();
    void pushFalse();
    void pushNull();
    void pushTrue();
    void storeVariable();
    void storeVariableExt();
    void subtract();

    Chunk chunk;
    const u8* ip;
    shell::Stack<DzValue, 512> stack;
    Interning interning;
};
