#include "dzobject.h"

#include <sh/utility.h>

#include "dzstring.h"

DzObject::DzObject(DzObject::Type type) : type(type) {}

DzObject::operator bool() const {
  switch (type) {
    case Type::String:
      return static_cast<bool>(as<DzString>());
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzObject::operator==(const DzObject& other) const -> bool {
  return this == &other;
}

auto DzObject::operator!=(const DzObject& other) const -> bool {
  return !(this == &other);
}

std::string_view DzObject::typeName() const {
  switch (type) {
    case Type::String:
      return as<DzString>().typeName();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

template <typename T>
  requires std::is_base_of_v<DzObject, T>
auto DzObject::as() const -> const T& {
  return *static_cast<const T*>(this);
}
