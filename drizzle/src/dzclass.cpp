#include "dzclass.h"

#include <sh/fmt.h>

#include "dzinstance.h"
#include "gc.h"

DzClass::DzClass(DzString* identifier)
  : DzObject(Type::Class), identifier(identifier) {}

DzClass::operator bool() const {
  return true;
}

auto DzClass::kind() const -> std::string_view {
  return "class";
}

auto DzClass::repr() const -> std::string {
  return fmt::format("<class {}>", identifier->repr());
}

void DzClass::add(DzFunction* function) {
  if (function->identifier->data == kInit) {
    init = function;
  }
  functions.insert_or_assign(function->identifier, function);
}

auto DzClass::get(DzString* identifier) -> DzFunction* {
  const auto iter = functions.find(identifier);
  return iter != functions.end()
    ? iter->second
    : nullptr;
}
