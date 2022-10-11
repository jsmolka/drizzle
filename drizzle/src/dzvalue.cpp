#include "dzvalue.h"

#include <sh/utility.h>

#include "dzstring.h"

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
      return static_cast<std::size_t>(b);
    case DzValue::Type::Int:
      return static_cast<std::size_t>(i);
    case DzValue::Type::Float:
      return whole(f) ? static_cast<std::size_t>(f) : std::hash<dzfloat>{}(f);
    case DzValue::Type::Object:
      if (o->is(DzObject::Type::String)) {
        return o->as<DzString>()->hash;
      }
      [[fallthrough]];
    default:
      SH_UNREACHABLE;
      return 0;
  }
}

auto DzValue::is(Type type) const -> bool {
  return this->type == type;
}

auto DzValue::isUndefined() const -> bool {
  return is(Type::Object) && o == nullptr;
}

auto DzValue::isHashable() const -> bool {
  return !is(Type::Object) || o->is(DzObject::Type::String);
}

auto DzValue::subscriptGet(Vm& vm, const DzValue& expr) const -> std::optional<DzValue> {
  switch (type) {
    case Type::Bool:
    case Type::Int:
    case Type::Float:
      return std::nullopt;
    case Type::Object:
      return o->subscriptGet(vm, expr);
    default:
      SH_UNREACHABLE;
      return std::nullopt;
  }
}
