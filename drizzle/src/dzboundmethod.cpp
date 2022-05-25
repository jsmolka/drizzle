#include "dzboundmethod.h"

DzBoundMethod::DzBoundMethod(DzObject* self, DzFunction* function)
  : DzObject(Type::BoundMethod), self(self), function(function) {}

DzBoundMethod::operator bool() const {
  return true;
}

auto DzBoundMethod::operator==(const DzBoundMethod& other) const -> bool {
  return self == other.self && function == other.function;
}

auto DzBoundMethod::repr() const -> std::string {
  return function->repr();
}
