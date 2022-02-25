#pragma once

#include <sh/stack.h>

#include "dzfunction.h"
#include "error.h"
#include "token.h"

class Vm {
public:
  friend class DzBuiltIn;

  void interpret(DzFunction* function);

private:
  static constexpr auto kMaximumRecursionDepth = 1000;

  struct Frame {
    u8* pc;
    std::size_t sp;
    DzFunction* function;
  };

  template<std::integral Integral>
  auto read() -> Integral;
  template<typename... Args>
  void raise(std::string_view format, Args&&... args);

  template<template<typename> typename Promote = promote_t, typename Callback>
  void unary(std::string_view operation, Callback callback);
  template<template<typename, typename> typename Promote = promote_t, typename Callback>
  void binary(std::string_view operation, Callback callback);

  void add();
  void bitwiseAnd();
  void bitwiseAsr();
  void bitwiseComplement();
  void bitwiseLsl();
  void bitwiseLsr();
  void bitwiseOr();
  void bitwiseXor();
  void call();
  template<typename Integral>
  void constant();
  void divide();
  void divideInt();
  void equal();
  void false_();
  void greater();
  void greaterEqual();
  void jump();
  void jumpFalse();
  void jumpFalsePop();
  void jumpTrue();
  void less();
  void lessEqual();
  template<typename Integral>
  void load();
  template<typename Integral>
  void loadAbsolute();
  void modulo();
  void multiply();
  void negate();
  void not_();
  void notEqual();
  void null();
  void pop();
  template<typename Integral>
  void popMultiple();
  void power();
  void return_();
  template<typename Integral>
  void store();
  template<typename Integral>
  void storeAbsolute();
  void subtract();
  void true_();

  Frame frame;
  sh::stack<Frame, 32> frames;
  sh::stack<DzValue, 512> stack;
};
