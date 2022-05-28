#pragma once

#include <sh/stack.h>

#include "dzclass.h"
#include "dzfunction.h"
#include "dzstring.h"
#include "error.h"
#include "token.h"

class Gc;

class Vm {
public:
  friend class DzFunction;
  friend class Gc;

  Vm(Gc& gc);

  void interpret(DzFunction* main);

private:
  static constexpr auto kMaximumRecursionDepth = 1000;

  struct Frame {
    u8* pc;
    std::size_t sp;
    DzFunction* function;
  };

  void defineNatives();
  void defineListMembers();
  void defineStringMembers();

  template<std::integral Integral>
  auto read() -> Integral;

  template<typename... Args>
  void raise(Args&&... args) {
    const auto line = frames.top().function->chunk().line(opcode_pc);
    throw RuntimeError(Location{line}, std::forward<Args>(args)...);
  }

  void expect(const DzValue& value, DzValue::Type type);
  void expect(const DzValue& value, DzObject::Type type);

  template<template<typename> typename Promote = promote_t, typename Callback>
  void unary(std::string_view operation, Callback callback);
  template<template<typename, typename> typename Promote = promote_t, typename Callback>
  void binary(std::string_view operation, Callback callback);

  auto forward(const DzValue& iteree) -> DzValue;
  auto reverse(const DzValue& iteree) -> DzValue;

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
  void call(DzFunction* function, std::size_t argc);
  template<std::integral Integral>
  void constant();
  void divide();
  void divideInt();
  void equal();
  void false_();
  void get();
  void greater();
  void greaterEqual();
  void in();
  void invoke();
  void iterForward();
  template<std::integral Integral>
  void iterAdvance();
  template<std::integral Integral>
  void iterDereference();
  void jump();
  void jumpFalse();
  void jumpFalsePop();
  void jumpTrue();
  void less();
  void lessEqual();
  template<std::integral Integral>
  void list();
  template<std::integral Integral>
  void load();
  template<std::integral Integral>
  void loadGlobal();
  void modulo();
  void multiply();
  void negate();
  void not_();
  void notEqual();
  void null_();
  void pop();
  template<std::integral Integral>
  void popMultiple();
  void power();
  void return_();
  void set();
  template<std::integral Integral>
  void store();
  template<std::integral Integral>
  void storeGlobal();
  void subscriptGet();
  void subscriptSet();
  void subtract();
  void true_();

  Gc& gc;
  u8* opcode_pc = nullptr;
  sh::stack<Frame, 32> frames;
  sh::stack<DzValue, 512> stack;
  sh::vector<DzValue, 256> globals;
  Map<DzFunction*> members[int(DzObject::Type::LastEnumValue)];
};
