#include "dzobject.h"

#include <sh/utility.h>

#include "dzbuiltin.h"
#include "dzfunction.h"
#include "dzstring.h"

DzObject::DzObject(Type type)
  : type(type) {}

DzObject::operator bool() const {
  switch (type) {
    case Type::BuiltIn:  return as<DzBuiltIn>();
    case Type::Function: return as<DzFunction>();
    case Type::String:   return as<DzString>();
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzObject::operator==(const DzObject& other) const -> bool {
  if (type != other.type) {
    return false;
  }

  switch (type) {
    case Type::BuiltIn:
    case Type::Function:
      return this == &other;
    case Type::String:
      return as<DzString>() == other.as<DzString>();
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzObject::operator!=(const DzObject& other) const -> bool {
  return !(*this == other);
}

auto DzObject::repr() const -> std::string {
  switch (type) {
    case Type::BuiltIn:  return as<DzBuiltIn>().repr();
    case Type::Function: return as<DzFunction>().repr();
    case Type::String:   return as<DzString>().repr();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

auto DzObject::name() const -> std::string_view {
  switch (type) {
    case Type::BuiltIn:  return as<DzBuiltIn>().name();
    case Type::Function: return as<DzFunction>().name();
    case Type::String:   return as<DzString>().name();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

template<typename T>
auto DzObject::as() const -> const T& {
  return *static_cast<const T*>(this);
}
