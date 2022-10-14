#pragma once

#include <sh/stack.h>

#include "arguments.h"
#include "dzfunction.h"
#include "dzstring.h"
#include "error.h"
#include "token.h"

class Gc;

class NotSupportedException {};

class Vm {
public:
  friend class DzFunction;
  friend class Gc;

  Vm(Gc& gc, const Arguments& arguments = {});

  void interpret(DzFunction* main);

public:  // Todo: ugly
  static constexpr auto kMaximumRecursionDepth = 1000;

  struct Frame {
    u8* pc;
    std::size_t sp;
    DzFunction* function;
  };

  void defineNatives();
  void defineBytesMembers();
  void defineListMembers();
  void defineMapMembers();
  void defineSdlWindowMembers();
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
  void expectHashable(const DzValue& value);

  template<template<typename> typename Promote = promote_t, typename Callback>
  void unary(std::string_view operation, Callback callback);
  template<typename Operation, template<typename, typename> typename Promote = promote_t, typename... Args>
  void binary(std::string_view operation, Args&&... args);

  auto forward(const DzValue& iteree) -> DzIterator*;
  auto reverse(const DzValue& iteree) -> DzIterator*;

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
  void iterCurrent();
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

  Gc& gc;
  Arguments arguments;
  u8* opcode_pc = nullptr;
  sh::stack<Frame, 32> frames;
  sh::stack<DzValue, 512> stack;
  sh::vector<DzValue, 256> globals;
  Map<DzFunction*> members[int(DzObject::Type::LastEnumValue)];
};
