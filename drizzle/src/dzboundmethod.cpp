#include "dzboundmethod.h"

DzBoundMethod::DzBoundMethod(DzObject* self, DzFunction* function)
  : DzObject(Type::BoundMethod), self(self), function(function) {}

DzBoundMethod::operator bool() const {
  return true;
}

auto DzBoundMethod::kind() const -> std::string_view {
  return function->kind();
}

auto DzBoundMethod::repr() const -> std::string {
  return function->repr();
}
