#include "dzvalue.h"

#include <sh/utility.h>

DzValue::DzValue()
  : type(Type::Undefined) {}

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

auto DzValue::name() const -> std::string_view {
  switch (type) {
    case Type::Bool:   return "bool";
    case Type::Int:    return "int";
    case Type::Float:  return "float";
    case Type::Object: return o->name();
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
