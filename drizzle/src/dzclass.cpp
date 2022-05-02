#include "dzclass.h"

#include <sh/fmt.h>

DzClass::DzClass(DzString* identifier)
  : DzObject(Type::Class), identifier(identifier) {}

DzClass::operator bool() const {
  return true;
}

auto DzClass::repr() const -> std::string {
  return fmt::format("<class {}>", name());
}

auto DzClass::name() const -> std::string_view {
  return identifier->data;
}
