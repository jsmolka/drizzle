#pragma once

#include <sh/stack.h>

#include "dzfunction.h"
#include "dzstring.h"
#include "error.h"
#include "map.h"
#include "program.h"
#include "token.h"

class Gc;

class NotFoundException {};
class NotSupportedException {};

class Vm {
public:
  struct Frame {
    u8* pc;
    std::size_t sp;
    DzFunction* function;
  };

  Vm(Gc& gc);

  void interpret(const Program& program);

  template<typename... Args>
  void raise(Args&&... args) {
    const auto line = frames.top().function->chunk().line(pc);
    throw RuntimeError(Location{line}, std::forward<Args>(args)...);
  }

  void expect(const DzValue& value, DzValue::Type type);
  void expect(const DzValue& value, DzObject::Type type);
  void expectArity(DzFunction::Arity expected, std::size_t got);

  Gc& gc;
  sh::stack<Frame> frames;
  sh::stack<DzValue> stack;
  sh::vector<DzValue> globals;
  StringMap<DzFunction*> members[int(DzObject::Type::LastEnumValue)];

private:
  static constexpr auto kMaximumRecursionDepth = 1000;

  void defineNatives();

  template<std::integral Integral>
  auto read() -> Integral;

  template<template<typename> typename Promote = promote_t, typename Callback>
  void unary(std::string_view operation, Callback callback);
  template<template<typename> typename Promote = promote_t, typename Callback>
  void binary(std::string_view operation, Callback callback);

  auto forward(DzValue& iteree) -> DzValue;
  auto reverse(DzValue& iteree) -> DzValue;

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
  void iterInit();
  template<std::integral Integral>
  void iterAdvance();
  template<std::integral Integral>
  void iterValue();
  void jump();
  void jumpFalse();
  void jumpFalsePop();
  void jumpTrue();
  void jumpTruePop();
  void less();
  void lessEqual();
  template<std::integral Integral>
  void list();
  template<std::integral Integral>
  void load();
  template<std::integral Integral>
  void loadGlobal();
  template<std::integral Integral>
  void map();
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
  void range();
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

  Program program;
  u8* pc = nullptr;
};
