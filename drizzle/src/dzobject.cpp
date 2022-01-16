#include "dzobject.h"

#include <sh/utility.h>

#include "dzstring.h"

DzObject::DzObject(Type type)
  : type(type) {}

DzObject::operator bool() const {
  switch (type) {
    case Type::String:
      return as<DzString>();
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzObject::operator==(const DzObject& other) const -> bool {
  return this == &other;
}

auto DzObject::operator!=(const DzObject& other) const -> bool {
  return this != &other;
}

auto DzObject::repr() const -> std::string {
  switch (type) {
    case Type::String:
      return as<DzString>().repr();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

auto DzObject::name() const -> std::string_view {
  switch (type) {
    case Type::String:
      return as<DzString>().name();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

template<typename T>
auto DzObject::as() const -> const T& {
  return *static_cast<const T*>(this);
}
