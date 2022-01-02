#include "dzvalue.h"

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
