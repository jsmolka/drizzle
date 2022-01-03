#pragma once

#include <sh/stack.h>

#include "chunk.h"
#include "stringpool.h"
#include "token.h"

class Vm {
public:
  Vm(StringPool& pool);

  void interpret(const Chunk& chunk);

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
  void bitwiseAnd();
  void bitwiseAsr();
  void bitwiseComplement();
  void bitwiseLsl();
  void bitwiseLsr();
  void bitwiseOr();
  void bitwiseXor();
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
  void storeVariable();
  void subtract();

  const u8* pc;
  const Chunk* chunk;
  StringPool& pool;
  sh::stack<DzValue, 512> stack;
};
