#pragma once

#include <sh/stack.h>

#include "dzfunction.h"
#include "error.h"
#include "token.h"

template<typename T>
concept UnaryHandler = requires(T&& handler, DzValue& dst, DzObject* obj) {
  { std::invoke(std::forward<T>(handler), dst, obj) } -> std::same_as<bool>;
  { std::invoke(std::forward<T>(handler), dst, dzint{}) } -> std::same_as<bool>;
  { std::invoke(std::forward<T>(handler), dst, dzbool{}) } -> std::same_as<bool>;
  { std::invoke(std::forward<T>(handler), dst, dzfloat{}) } -> std::same_as<bool>;
};

template<typename T>
concept BinaryHandler = requires(T&& handler, DzValue& dst, DzObject* obj) {
  { std::invoke(std::forward<T>(handler), dst, obj, obj) } -> std::same_as<bool>;
  { std::invoke(std::forward<T>(handler), dst, dzint{}, dzint{}) } -> std::same_as<bool>;
  { std::invoke(std::forward<T>(handler), dst, dzbool{}, dzbool{}) } -> std::same_as<bool>;
  { std::invoke(std::forward<T>(handler), dst, dzfloat{}, dzfloat{}) } -> std::same_as<bool>;
};

class Vm {
public:
  void interpret(DzFunction* function);

private:
  struct Frame {
    DzFunction* function;
    u8* pc;
    std::size_t sp;
  };

  template<std::integral Integral>
  Integral read();

  template<typename Error, typename... Args>
    requires std::is_base_of_v<RuntimeError, Error>
  void raise(std::string_view message, Args&&... args);

  template<template<typename T> typename Promote = promote_t, UnaryHandler Handler>
  void unary(std::string_view operation, Handler callback);
  template<template<typename T, typename U> typename Promote = promote_t, BinaryHandler Handler>
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

  sh::stack<DzValue, 512> stack;
  sh::stack<Frame, 128> frames;
};
