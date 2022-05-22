#include "dzvalue.h"

#include <sh/utility.h>

DzValue::DzValue()
  : type(Type::Object), o(nullptr) {}

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

auto DzValue::operator==(const DzValue& other) const -> bool {
  return binary(*this, other, []<typename A, typename B>(const A& a, const B& b) {
    if constexpr (dz_primitive<A, B>) {
      return a == b;
    } else if constexpr (dz_object<A, B>) {
      return *a == *b;
    } else {
      return false;
    }
  });
}

auto DzValue::operator!=(const DzValue& other) const -> bool {
  return !(*this == other);
}

auto DzValue::kind() const -> std::string_view {
  return type == Type::Object
    ? fmt::formatter<DzObject::Type>::repr(o->type)
    : fmt::formatter<Type>::repr(type);
}

auto DzValue::repr() const -> std::string {
  auto whole = [](double value) {
    return std::fmod(value, 1.0) == 0.0;
  };

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

auto DzValue::is(Type type) const -> bool {
  return this->type == type;
}

auto DzValue::is(DzObject::Type type) const -> bool {
  return is(Type::Object) && o->is(type);
}

auto DzValue::isUndefined() const -> bool {
  return is(Type::Object) && o == nullptr;
}
