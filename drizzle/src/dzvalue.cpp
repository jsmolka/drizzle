#include "dzvalue.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzValue::DzValue()
  : type(Type::Null) {}

DzValue::operator bool() const {
  switch (type) {
    case Type::Bool:   return b;
    case Type::Int:    return static_cast<bool>(i);
    case Type::Float:  return static_cast<bool>(f);
    case Type::Null:   return false;
    case Type::Object: return static_cast<bool>(*o);
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzValue::repr() const -> std::string {
  auto whole = [](double v) {
    return std::fmod(v, 1.0) == 0.0;
  };

  switch (type) {
    case DzValue::Type::Bool:   return fmt::to_string(b);
    case DzValue::Type::Int:    return fmt::to_string(i);
    case DzValue::Type::Float:  return fmt::format(fmt::runtime(whole(f) ? "{:.1f}" : "{}"), f);
    case DzValue::Type::Object: return o->repr();
    case DzValue::Type::Null:   return "null";
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

auto DzValue::typeName() const -> std::string_view {
  switch (type) {
    case Type::Bool:   return "bool";
    case Type::Int:    return "int";
    case Type::Float:  return "float";
    case Type::Null:   return "null";
    case Type::Object: return o->typeName();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}
