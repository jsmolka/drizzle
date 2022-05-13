#include "dznull.h"

DzNull::DzNull()
  : DzObject(Type::Null) {}

DzNull::operator bool() const {
  return false;
}

auto DzNull::kind() const -> std::string_view {
  return "null";
}

auto DzNull::repr() const -> std::string {
  return "null";
}
