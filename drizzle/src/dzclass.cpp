#include "dzclass.h"

#include "gc.h"

DzClass::DzClass(DzString* identifier)
  : DzObject(Type::Class), identifier(identifier) {}

auto DzClass::repr() const -> std::string {
  return fmt::format("<class {}>", identifier->data);
}

void DzClass::mark(Gc& gc) {
  DzObject::mark(gc);
  gc.mark(identifier);
  for (const auto& [key, value] : functions) {
    gc.mark(key);
    gc.mark(value);
  }
}

auto DzClass::get(const DzString* identifier) const -> DzFunction* {
  return functions.get(identifier).value_or(nullptr);
}

void DzClass::set(const DzString* identifier, DzFunction* function) {
  if (identifier->data == kInit) {
    init = function;
  }
  functions.set(identifier, function);
}
