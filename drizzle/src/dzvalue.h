#pragma once

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzobject.h"
#include "dzprimitives.h"

class DzValue {
public:
  enum class Type { Bool, Int, Float, Object, LastEnumValue };

  DzValue();
  template<typename T>
    requires dz_primitive<T> || dz_object<T>
  DzValue(const T& value) {
    *this = value;
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
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;

  auto is(Type type) const -> bool;
  auto is(DzObject::Type type) const -> bool;
  auto isUndefined() const -> bool;

  template<typename T>
    requires std::is_base_of_v<DzObject, T>
  auto as() const -> T* {
    assert(is(Type::Object));
    return static_cast<T*>(o);
  }

  Type type;
  union {
    dzbool b;
    dzint i;
    dzfloat f;
    DzObject* o;
  };
};

template<>
struct fmt::formatter<DzValue> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const DzValue& value, FormatContext& ctx) const {
    return fmt::formatter<std::string>::format(value.repr(), ctx);
  }
};

template<>
struct fmt::formatter<DzValue::Type> : fmt::formatter<std::string_view> {
  static_assert(int(DzValue::Type::LastEnumValue) == 4);
  template<typename FormatContext>
  auto format(const DzValue::Type& type, FormatContext& ctx) const {
    auto repr = [](const DzValue::Type& type) {
      switch (type) {
        case DzValue::Type::Bool:   return "bool";
        case DzValue::Type::Int:    return "int";
        case DzValue::Type::Float:  return "float";
        case DzValue::Type::Object: return "object";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(type), ctx);
  }
};
