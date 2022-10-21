#include "dzboundmethod.h"

#include "gc.h"

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

void DzBoundMethod::mark(Gc& gc) {
  DzObject::mark(gc);
  gc.mark(self);
  gc.mark(function);
}
