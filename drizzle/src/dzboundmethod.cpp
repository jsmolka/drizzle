#include "dzboundmethod.h"

DzBoundMethod::DzBoundMethod(DzObject* self, DzFunction* function)
  : DzObject(Type::BoundMethod), self(self), function(function) {}

auto DzBoundMethod::operator==(const DzObject& other) const -> bool {
  return other.type == Type::BoundMethod &&
    other.as<DzBoundMethod>()->self == self &&
    other.as<DzBoundMethod>()->function == function;
}

auto DzBoundMethod::repr() const -> std::string {
  return function->repr();
}
