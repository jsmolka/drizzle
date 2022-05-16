#include "dzclass.h"

#include <sh/fmt.h>

#include "dzinstance.h"
#include "gc.h"

DzClass::DzClass(DzString* identifier)
  : DzObject(Type::Class), identifier(identifier) {}

DzClass::operator bool() const {
  return true;
}

auto DzClass::repr() const -> std::string {
  return fmt::format("<class {}>", identifier->repr());
}

auto DzClass::get(DzString* identifier) -> DzFunction* {
  const auto iter = functions.find(identifier);
  return iter != functions.end()
    ? iter->second
    : nullptr;
}

void DzClass::set(DzString* identifier, DzFunction* function) {
  if (identifier->data == kInit) {
    init = function;
  }
  functions.insert_or_assign(identifier, function);
}
