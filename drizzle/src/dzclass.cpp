#include "dzclass.h"

#include <sh/fmt.h>

DzClass::DzClass(const std::string& identifier)
  : DzObject(Type::Class), identifier(identifier) {}

DzClass::operator bool() const {
  return true;
}

auto DzClass::repr() const -> std::string {
  return fmt::format("<class {}>", identifier);
}

auto DzClass::name() const -> std::string_view {
  return "class";
}
