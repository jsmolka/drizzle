#pragma once

#include <shell/stack.h>

#include "dzclosure.h"
#include "dzfunction.h"
#include "interning.h"
#include "token.h"

class Vm
{
public:
    void interpret(const Tokens& tokens);

private:
    struct CallFrame  // Todo: stack frame?
    {
        DzClosure* closure;
        const u8* pc;
        std::size_t sp;
    };

    template<typename Integral>
    Integral read();
    DzUpvalue* capture(std::size_t index);

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
    void call(DzClosure* closure, u8 argc);
    void callValue();
    template<typename Integral>
    void closure();
    template<typename Integral>
    void constant();
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
    template<typename Integral>
    void loadUpvalue();
    template<typename Integral>
    void loadVariable();
    void modulo();
    void multiply();
    void negate();
    void not_();
    void notEqual();
    void pop();
    template<typename Integral>
    void popMultiple();
    void power();
    void print();
    void pushFalse();
    void pushNull();
    void pushTrue();
    bool return_();
    template<typename Integral>
    void storeUpvalue();
    template<typename Integral>
    void storeVariable();
    void subtract();

    CallFrame* frame;
    shell::Stack<DzValue, 512> stack;
    shell::Vector<CallFrame, 64> frames;
    Interning interning;
};
