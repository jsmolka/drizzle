#include "dzclass.h"

#include <sh/fmt.h>

DzClass::DzClass(DzString* identifier)
  : DzObject(Type::Class), identifier(identifier) {}

auto DzClass::repr() const -> std::string {
  return fmt::format("<class {}>", identifier->data);
}

auto DzClass::get(const DzString* identifier) -> DzFunction* {
  const auto iter = functions.find(identifier);
  return iter != functions.end()
    ? iter->second
    : nullptr;
}

void DzClass::set(const DzString* identifier, DzFunction* function) {
  if (identifier->data == kInit) {
    init = function;
  }
  functions.insert_or_assign(identifier, function);
}
