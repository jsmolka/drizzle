#pragma once

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzobject.h"
#include "dzprimitives.h"

template<typename A, template<typename> typename Promote>
using unary_t = std::conditional_t<dz_primitive<A>, Promote<A>, A>;

template<typename A, typename B, template<typename, typename> typename Promote>
using binary_t = std::conditional_t<dz_primitive<A, B>, Promote<A, B>, A>;

class DzValue {
public:
  enum class Type { Bool, Int, Float, Object, LastEnumValue };

  DzValue();
  template<typename T>
    requires dz_primitive<T> || dz_object<T>
  DzValue(const T& value) {
    *this = value;
  }

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

  template<template<typename, typename> typename Promote = promote_t, typename Callback>
  static auto binary(const DzValue& a, const DzValue& b, Callback callback) {
    static_assert(int(Type::LastEnumValue) == 4);

    #define DZ_HASH(a, b) int(Type::LastEnumValue) * int(a) + int(b)
    #define DZ_EVAL(a, b)                  \
    {                                      \
      using A  = decltype(a);              \
      using B  = decltype(b);              \
      using AP = binary_t<A, B, Promote>;  \
      using BP = binary_t<B, A, Promote>;  \
      return callback(AP(a), BP(b));       \
    }

    switch (DZ_HASH(a.type, b.type)) {
      case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Bool  ): DZ_EVAL(a.b, b.b);
      case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Int   ): DZ_EVAL(a.b, b.i);
      case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Float ): DZ_EVAL(a.b, b.f);
      case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Object): DZ_EVAL(a.b, b.o);
      case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Bool  ): DZ_EVAL(a.i, b.b);
      case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Int   ): DZ_EVAL(a.i, b.i);
      case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Float ): DZ_EVAL(a.i, b.f);
      case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Object): DZ_EVAL(a.i, b.o);
      case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Bool  ): DZ_EVAL(a.f, b.b);
      case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Int   ): DZ_EVAL(a.f, b.i);
      case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Float ): DZ_EVAL(a.f, b.f);
      case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Object): DZ_EVAL(a.f, b.o);
      case DZ_HASH(DzValue::Type::Object, DzValue::Type::Bool  ): DZ_EVAL(a.o, b.b);
      case DZ_HASH(DzValue::Type::Object, DzValue::Type::Int   ): DZ_EVAL(a.o, b.i);
      case DZ_HASH(DzValue::Type::Object, DzValue::Type::Float ): DZ_EVAL(a.o, b.f);
      case DZ_HASH(DzValue::Type::Object, DzValue::Type::Object): DZ_EVAL(a.o, b.o);
      default:
        SH_UNREACHABLE;
        return callback(0, 0);
    }

    #undef DZ_EVAL
    #undef DZ_HASH
  }

  template<typename T>
    requires dz_primitive<T> || dz_object<T>
  auto operator=(const T& value) -> DzValue& {
    if constexpr (dz_bool<T>) {
      b = value;
      type = Type::Bool;
    } else if constexpr (dz_int<T>) {
      i = value;
      type = Type::Int;
    } else if constexpr (dz_float<T>) {
      f = value;
      type = Type::Float;
    } else if constexpr (dz_object<T>) {
      o = value;
      type = Type::Object;
    }
    return *this;
  }

  operator bool() const;
  auto operator==(const DzValue& other) const -> bool;
  auto operator!=(const DzValue& other) const -> bool;
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;
  auto hash() const -> std::size_t;

  auto is(Type type) const -> bool;
  auto is(DzObject::Type type) const -> bool;
  auto isUndefined() const -> bool;
  auto isHashable() const -> bool;

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
