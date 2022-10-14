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

struct DzEqual {
  template<typename A, typename B>
  auto operator()(const A& a, const B& b) const -> DzValue {
    if constexpr (dz_primitive<A, B>) {
      return a == b;
    } else if constexpr (dz_object<A, B>) {
      return *a == *b;
    }
    return false;
  }
};

auto DzValue::operator==(const DzValue& other) const -> bool {
  return binary<DzEqual>(*this, other);
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

auto DzValue::isObject() const -> bool {
  return type == Type::Object;
}

auto DzValue::isUndefined() const -> bool {
  return is(Type::Object) && o == nullptr;
}

auto DzValue::isHashable() const -> bool {
  return !is(Type::Object) || o->is(DzObject::Type::String);
}

auto DzValue::subscriptGet(Vm& vm, const DzValue& expr) -> DzValue {
  if (isObject()) {
    return o->subscriptGet(vm, expr);
  } else {
    throw NotSupportedException();
  }
}

void DzValue::subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value) {
  if (isObject()) {
    o->subscriptSet(vm, expr, value);
  } else {
    throw NotSupportedException();
  }
}
