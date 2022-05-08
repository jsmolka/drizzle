#pragma once

#include <sh/stack.h>

#include "dzfunction.h"
#include "dzstring.h"
#include "error.h"
#include "token.h"

class Compiler;
class Gc;

class Vm {
public:
  friend class Compiler;
  friend class DzFunction;
  friend class Gc;

  Vm(Gc& gc);

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
  void raise(std::string_view format, Args&&... args) {
    const auto& frame = frames.top();
    const auto line = frame.function->chunk().line(opcode_pc);
    throw RuntimeError(Location{line}, format, std::forward<Args>(args)...);
  }

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
  void call(DzValue& callee, std::size_t argc);
  template<typename Integral>
  void constant();
  void divide();
  void divideInt();
  void equal();
  void false_();
  void get();
  void greater();
  void greaterEqual();
  void invoke();
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
  void null_();
  void pop();
  template<typename Integral>
  void popMultiple();
  void power();
  void return_();
  void set();
  template<typename Integral>
  void store();
  template<typename Integral>
  void storeAbsolute();
  void subtract();
  void true_();

  Gc& gc;
  u8* opcode_pc = nullptr;
  sh::stack<Frame, 32> frames;
  sh::stack<DzValue, 512> stack;
};
