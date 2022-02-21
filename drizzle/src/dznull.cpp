#include "dznull.h"

DzNull::DzNull()
  : DzObject(Type::Null) {}

DzNull::operator bool() const {
  return false;
}

auto DzNull::repr() const -> std::string {
  return "null";
}

auto DzNull::name() const -> std::string_view {
  return "null";
}
