#pragma once

#include <sh/stack.h>

#include "dzfunction.h"
#include "dzstring.h"
#include "error.h"
#include "map.h"
#include "opcode.h"
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
  [[noreturn]] void raise(Args&&... args) {
    const auto line = frame.function->chunk().line(opcode_pc);
    throw RuntimeError(Location{line}, std::forward<Args>(args)...);
  }

  void expect(const DzValue& value, DzValue::Type type);
  void expect(const DzValue& value, DzObject::Type type);
  void expectArity(DzFunction::Arity expected, std::size_t got);

  Gc& gc;
  Frame frame;
  sh::stack<Frame> frames;
  sh::stack<DzValue, 256> stack;
  sh::vector<DzValue> globals;
  StringMap<DzFunction*> members[int(DzObject::Type::LastEnumValue)];

private:
  static constexpr auto kMaximumRecursionDepth = 1000;

  void defineNatives();

  template<std::integral Integral>
  SH_INLINE auto read() -> Integral;
  SH_INLINE void patch(Opcode opcode);

  SH_INLINE auto checkType(const DzValue& a, DzValue::Type type) -> bool;
  SH_INLINE auto checkType(const DzValue& a, const DzValue& b, DzValue::Type type) -> bool;

  template<template<typename> typename Promote = promote_t, typename Callback>
  SH_INLINE void unary(std::string_view operation, Callback callback);
  template<template<typename> typename Promote = promote_t, typename Callback>
  SH_INLINE void binary(std::string_view operation, Callback callback);

  auto forward(DzValue& iteree) -> DzValue;
  auto reverse(DzValue& iteree) -> DzValue;

  SH_INLINE void add();
  SH_INLINE void addGeneric();
  SH_INLINE void addInt();
  SH_INLINE void addFloat();
  SH_INLINE void bitwiseAnd();
  SH_INLINE void bitwiseAndGeneric();
  SH_INLINE void bitwiseAndInt();
  SH_INLINE void bitwiseAsr();
  SH_INLINE void bitwiseAsrGeneric();
  SH_INLINE void bitwiseAsrInt();
  SH_INLINE void bitwiseComplement();
  SH_INLINE void bitwiseComplementGeneric();
  SH_INLINE void bitwiseComplementInt();
  SH_INLINE void bitwiseLsl();
  SH_INLINE void bitwiseLslGeneric();
  SH_INLINE void bitwiseLslInt();
  SH_INLINE void bitwiseLsr();
  SH_INLINE void bitwiseLsrGeneric();
  SH_INLINE void bitwiseLsrInt();
  SH_INLINE void bitwiseOr();
  SH_INLINE void bitwiseOrGeneric();
  SH_INLINE void bitwiseOrInt();
  SH_INLINE void bitwiseXor();
  SH_INLINE void bitwiseXorGeneric();
  SH_INLINE void bitwiseXorInt();
  SH_INLINE void call();
  SH_INLINE void call(DzValue& callee, std::size_t argc);
  SH_INLINE void call(DzFunction* function, std::size_t argc);
  template<std::integral Integral>
  SH_INLINE void constant();
  SH_INLINE void divide();
  SH_INLINE void divideGeneric();
  SH_INLINE void divideFloat();
  SH_INLINE void divideInteger();
  SH_INLINE void divideIntegerGeneric();
  SH_INLINE void divideIntegerInt();
  SH_INLINE void divideIntegerFloat();
  SH_INLINE void equal();
  SH_INLINE void equalGeneric();
  SH_INLINE void equalInt();
  SH_INLINE void equalFloat();
  SH_INLINE void false_();
  SH_INLINE void get();
  SH_INLINE void greater();
  SH_INLINE void greaterGeneric();
  SH_INLINE void greaterInt();
  SH_INLINE void greaterFloat();
  SH_INLINE void greaterEqual();
  SH_INLINE void greaterEqualGeneric();
  SH_INLINE void greaterEqualInt();
  SH_INLINE void greaterEqualFloat();
  SH_INLINE void in();
  SH_INLINE void invoke();
  SH_INLINE void iterInit();
  template<std::integral Integral>
  SH_INLINE void iterAdvance();
  template<std::integral Integral>
  SH_INLINE void iterValue();
  SH_INLINE void jump();
  SH_INLINE void jumpFalse();
  SH_INLINE void jumpFalsePop();
  SH_INLINE void jumpTrue();
  SH_INLINE void jumpTruePop();
  SH_INLINE void less();
  SH_INLINE void lessGeneric();
  SH_INLINE void lessInt();
  SH_INLINE void lessFloat();
  SH_INLINE void lessEqual();
  SH_INLINE void lessEqualGeneric();
  SH_INLINE void lessEqualInt();
  SH_INLINE void lessEqualFloat();
  template<std::integral Integral>
  SH_INLINE void list();
  template<std::integral Integral>
  SH_INLINE void load();
  template<std::integral Integral>
  SH_INLINE void loadGlobal();
  template<std::integral Integral>
  SH_INLINE void map();
  SH_INLINE void modulo();
  SH_INLINE void moduloGeneric();
  SH_INLINE void moduloInt();
  SH_INLINE void moduloFloat();
  SH_INLINE void multiply();
  SH_INLINE void multiplyGeneric();
  SH_INLINE void multiplyInt();
  SH_INLINE void multiplyFloat();
  SH_INLINE void negate();
  SH_INLINE void negateGeneric();
  SH_INLINE void negateInt();
  SH_INLINE void negateFloat();
  SH_INLINE void not_();
  SH_INLINE void notGeneric();
  SH_INLINE void notBool();
  SH_INLINE void notInt();
  SH_INLINE void notFloat();
  SH_INLINE void notObject();
  SH_INLINE void notEqual();
  SH_INLINE void notEqualGeneric();
  SH_INLINE void notEqualInt();
  SH_INLINE void notEqualFloat();
  SH_INLINE void null_();
  SH_INLINE void pop();
  template<std::integral Integral>
  SH_INLINE void popMultiple();
  SH_INLINE void power();
  SH_INLINE void powerGeneric();
  SH_INLINE void powerInt();
  SH_INLINE void powerFloat();
  SH_INLINE void range();
  SH_INLINE void return_();
  SH_INLINE void set();
  template<std::integral Integral>
  SH_INLINE void store();
  template<std::integral Integral>
  SH_INLINE void storeGlobal();
  SH_INLINE void subscriptGet();
  SH_INLINE void subscriptSet();
  SH_INLINE void subtract();
  SH_INLINE void subtractGeneric();
  SH_INLINE void subtractInt();
  SH_INLINE void subtractFloat();
  SH_INLINE void switchCase();
  SH_INLINE void true_();

  Program program;
  u8* opcode_pc = nullptr;
};
