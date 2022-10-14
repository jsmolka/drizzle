#pragma once

#include <optional>

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzprimitives.h"

class DzObject;
// Todo: needed?
class Vm;

template<typename A, template<typename> typename Promote>
using unary_t = std::conditional_t<dz_primitive<A>, Promote<A>, A>;

template<typename A, typename B, template<typename, typename> typename Promote>
using binary_t = std::conditional_t<dz_primitive<A, B>, Promote<A, B>, A>;

class DzValue {
public:
  enum class Type { Bool, Int, Float, Object, LastEnumValue };

  DzValue();
  DzValue(dzbool value);
  DzValue(dzint value);
  DzValue(dzfloat value);
  DzValue(DzObject* value);

  auto operator=(dzbool value) -> DzValue&;
  auto operator=(dzint value) -> DzValue&;
  auto operator=(dzfloat value) -> DzValue&;
  auto operator=(DzObject* value) -> DzValue&;

  template<template<typename> typename Promote = promote_t, typename Callback>
  static auto unary(const DzValue& a, Callback callback) {
    static_assert(int(Type::LastEnumValue) == 4);

    #define DZ_EVAL(a)                 \
    {                                  \
      using A  = decltype(a);          \
      using AP = unary_t<A, Promote>;  \
      return callback(AP(a));          \
    }

    switch (a.type) {
      case DzValue::Type::Bool:   DZ_EVAL(a.b);
      case DzValue::Type::Int:    DZ_EVAL(a.i);
      case DzValue::Type::Float:  DZ_EVAL(a.f);
      case DzValue::Type::Object: DZ_EVAL(a.o);
      default:
        SH_UNREACHABLE;
        return callback(0);
    }

    #undef DZ_EVAL
  }

  template<typename Operation, template<typename, typename> typename Promote = promote_t, typename... Args>
  static auto binary(const DzValue& a, const DzValue& b, Args&&... args) {
    static_assert(int(Type::LastEnumValue) == 4);

    auto eval = [&args...]<typename A, typename B>(const A& a, const B& b) {
      return Operation{}.operator()(
        binary_t<A, B, Promote>(a),
        binary_t<B, A, Promote>(b),
        std::forward<Args>(args)...
      );  
    };

    switch (int(a.type) << 2 | int(b.type)) {
      case 0x0: return eval(a.b, b.b);
      case 0x1: return eval(a.b, b.i);
      case 0x2: return eval(a.b, b.f);
      case 0x3: return eval(a.b, b.o);
      case 0x4: return eval(a.i, b.b);
      case 0x5: return eval(a.i, b.i);
      case 0x6: return eval(a.i, b.f);
      case 0x7: return eval(a.i, b.o);
      case 0x8: return eval(a.f, b.b);
      case 0x9: return eval(a.f, b.i);
      case 0xA: return eval(a.f, b.f);
      case 0xB: return eval(a.f, b.o);
      case 0xC: return eval(a.o, b.b);
      case 0xD: return eval(a.o, b.i);
      case 0xE: return eval(a.o, b.f);
      case 0xF: return eval(a.o, b.o);
      default:
        SH_UNREACHABLE;
        return eval(nullptr, nullptr);
    };
  }

  operator bool() const;
  auto operator==(const DzValue& other) const -> bool;
  auto operator!=(const DzValue& other) const -> bool;
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;
  auto hash() const -> std::size_t;

  auto is(Type type) const -> bool;
  auto isUndefined() const -> bool;
  auto isHashable() const -> bool;

  auto subscriptGet(Vm& vm, const DzValue& expr) -> DzValue;

  Type type;
  union {
    dzbool b;
    dzint i;
    dzfloat f;
    DzObject* o;
  };
};

template<>
struct fmt::formatter<DzValue::Type> : fmt::formatter<std::string_view> {
  static auto repr(const DzValue::Type& type) -> std::string_view {
    static_assert(int(DzValue::Type::LastEnumValue) == 4);
    switch (type) {
      case DzValue::Type::Bool:   return "bool";
      case DzValue::Type::Int:    return "int";
      case DzValue::Type::Float:  return "float";
      case DzValue::Type::Object: return "object";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }
  
  template<typename FormatContext>
  auto format(const DzValue::Type& type, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(repr(type), ctx);
  }
};

namespace std {

template<>
struct hash<DzValue> {
  auto operator()(const DzValue& value) const -> std::size_t {
    return value.hash();
  }
};

}  // namespace std
