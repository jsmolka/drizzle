#pragma once

#include "dzobject.h"
#include "dzprimitives.h"

struct DzNull {};

template <typename... Ts>
inline constexpr auto is_dz_null_v = shell::is_same_v<DzNull, Ts...>;

template <typename... Ts>
struct is_dz_null : std::bool_constant<is_dz_null_v<Ts...>> {};

struct DzValue {
  enum class Type { Bool, Int, Float, Null, Object, LastEnumValue };

  DzValue();
  template <typename T>
  DzValue(const T& value);
  template <typename T>
  auto operator=(const T& value) -> DzValue&;

  auto typeName() const -> std::string_view;

  operator bool() const;

  Type type;
  union {
    dzbool b;
    dzint i;
    dzfloat f;
    DzObject* o;
  };
};

template <typename T>
DzValue::DzValue(const T& value) {
  *this = value;
}

template <typename T>
auto DzValue::operator=(const T& value) -> DzValue& {
  static_assert(is_dz_primitive_v<T> || is_dz_object_v<T>);

  if constexpr (is_dz_bool_v<T>) {
    b = value;
    type = Type::Bool;
  } else if constexpr (is_dz_int_v<T>) {
    i = value;
    type = Type::Int;
  } else if constexpr (is_dz_float_v<T>) {
    f = value;
    type = Type::Float;
  } else if constexpr (is_dz_object_v<T>) {
    o = value;
    type = Type::Object;
  }
  return *this;
}
