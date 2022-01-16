#pragma once

#include <sh/fmt.h>

#include "dzobject.h"
#include "dzprimitives.h"

struct DzNull {};

template<typename... Ts>
concept dz_null = std::conjunction_v<std::is_same<DzNull, Ts>...>;

class DzValue {
public:
  enum class Type { Bool, Int, Float, Null, Object, LastEnumValue };

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

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

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
