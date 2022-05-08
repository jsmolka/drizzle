#include "dzboundmethod.h"

DzBoundMethod::DzBoundMethod(DzObject* self, DzFunction* function)
  : DzObject(Type::BoundMethod), self(self), function(function) {}

DzBoundMethod::operator bool() const {
  return true;
}

auto DzBoundMethod::repr() const -> std::string {
  return function->repr();
}

auto DzBoundMethod::name() const -> std::string_view {
  return function->name();
}
