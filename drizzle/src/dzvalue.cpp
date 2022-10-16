#include "dzvalue.h"

#include <sh/utility.h>

#include "dzstring.h"
#include "vm.h"

inline auto whole(dzfloat value) -> bool {
  return std::fmod(value, 1.0) == 0.0;
}

DzValue::DzValue()
  : DzValue(nullptr) {}

DzValue::DzValue(dzbool value)
  : type(Type::Bool), b(value) {}

DzValue::DzValue(dzint value)
  : type(Type::Int), i(value) {}

DzValue::DzValue(dzfloat value)
  : type(Type::Float), f(value) {}

DzValue::DzValue(DzObject* value)
  : type(Type::Object), o(value) {}

auto DzValue::operator=(dzbool value) -> DzValue& {
  b = value;
  type = Type::Bool;
  return *this;
}

auto DzValue::operator=(dzint value) -> DzValue& {
  i = value;
  type = Type::Int;
  return *this;
}

auto DzValue::operator=(dzfloat value) -> DzValue& {
  f = value;
  type = Type::Float;
  return *this;
}

auto DzValue::operator=(DzObject* value) -> DzValue& {
  o = value;
  type = Type::Object;
  return *this;
}

DzValue::operator bool() const {
  switch (type) {
    case Type::Bool:   return b;
    case Type::Int:    return i;
    case Type::Float:  return f;
    case Type::Object: return *o;
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzValue::operator->() -> DzObject* {
  assert(o && isObject());
  return o;
}

auto DzValue::operator->() const -> const DzObject* {
  assert(o && isObject());
  return o;
}

auto DzValue::operator==(const DzValue& other) const -> bool {
  return binary(*this, other, []<typename A, typename B>(const A& a, const B& b) {
    if constexpr (dz_primitive<A, B>) {
      return a == b;
    } else if constexpr (dz_object<A, B>) {
      return *a == *b;
    }
    return false;
  });
}

auto DzValue::operator!=(const DzValue& other) const -> bool {
  return !(*this == other);
}

auto DzValue::kind() const -> std::string_view {
  if (isObject()) {
    return o->kind();
  } else {
    return fmt::formatter<Type>::repr(type);
  }
}

auto DzValue::repr() const -> std::string {
  switch (type) {
    case DzValue::Type::Bool:   return fmt::to_string(b);
    case DzValue::Type::Int:    return fmt::to_string(i);
    case DzValue::Type::Float:  return fmt::format(fmt::runtime(whole(f) ? "{:.1f}" : "{}"), f);
    case DzValue::Type::Object: return o->repr();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

auto DzValue::hash() const -> std::size_t {
  switch (type) {
    case DzValue::Type::Bool:
      return std::hash<dzint>{}(b);
    case DzValue::Type::Int:
      return std::hash<dzint>{}(i);
    case DzValue::Type::Float:
      return whole(f) ? std::hash<dzint>{}(f) : std::hash<dzfloat>{}(f);
    case DzValue::Type::Object:
      return std::hash<DzObject>{}(*o);
    default:
      SH_UNREACHABLE;
      return 0;
  }
}

auto DzValue::is(Type type) const -> bool {
  return this->type == type;
}

auto DzValue::isObject() const -> bool {
  return type == Type::Object;
}

auto DzValue::isUndefined() const -> bool {
  return isObject() && o == nullptr;
}
