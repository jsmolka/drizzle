#include "dzmethod.h"

DzMethod::DzMethod(DzObject* self, DzFunction* function)
  : DzObject(Type::Method), self(self), function(function) {}

DzMethod::operator bool() const {
  return true;
}

auto DzMethod::repr() const -> std::string {
  return function->repr();
}

auto DzMethod::name() const -> std::string_view {
  return function->name();
}
