#pragma once

#include <optional>

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzprimitives.h"

class DzObject;
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

  template<typename Functor, template<typename, typename> typename Promote = promote_t, typename... Args>
  static auto binary(const DzValue& a, const DzValue& b, Args&&... args) {
    static_assert(int(Type::LastEnumValue) == 4);

    #define DZ_EVAL(a, b)            \
    {                                \
      using A = decltype(a);         \
      using B = decltype(b);         \
      return Functor{}.operator()(   \
        binary_t<A, B, Promote>(a),  \
        binary_t<B, A, Promote>(b),  \
        std::forward<Args>(args)...  \
      );                             \
    }

    using Return = decltype(Functor{}.operator()(dzint{}, dzint{}, std::forward<Args>(args)...));
    using Binary = Return(*)(const DzValue&, const DzValue&, Args&&...);

    static constexpr Binary kBinaries[16] = {
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.b, b.b); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.b, b.i); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.b, b.f); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.b, b.o); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.i, b.b); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.i, b.i); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.i, b.f); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.i, b.o); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.f, b.b); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.f, b.i); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.f, b.f); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.f, b.o); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.o, b.b); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.o, b.i); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.o, b.f); },
      [](const DzValue& a, const DzValue& b, Args&&... args) -> Return { DZ_EVAL(a.o, b.o); },
    };
    return kBinaries[int(a.type) << 2 | int(b.type)](a, b, std::forward<Args>(args)...);

    #undef DZ_EVAL
  }

  auto operator=(std::same_as<dzbool> auto value) -> DzValue& {
    b = value;
    type = Type::Bool;
    return *this;
  }

  auto operator=(std::same_as<dzint> auto value) -> DzValue& {
    i = value;
    type = Type::Int;
    return *this;
  }

  auto operator=(std::same_as<dzfloat> auto value) -> DzValue& {
    f = value;
    type = Type::Float;
    return *this;
  }

  auto operator=(std::same_as<DzObject*> auto value) -> DzValue& {
    o = value;
    type = Type::Object;
    return *this;
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
