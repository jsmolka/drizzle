#include "dzclass.h"

#include <sh/fmt.h>

#include "dzinstance.h"
#include "gc.h"

DzClass::DzClass(DzString* identifier)
  : DzObject(Type::Class), identifier(identifier) {
  construct = [this](Gc& gc) {
    return gc.construct<DzInstance>(this);
  };
}

DzClass::operator bool() const {
  return true;
}

auto DzClass::repr() const -> std::string {
  return fmt::format("<class {}>", identifier->repr());
}

auto DzClass::name() const -> std::string_view {
  return "class";
}

void DzClass::add(DzFunction* function) {
  if (function->identifier->data == kInit) {
    init = function;
  }
  functions.insert_or_assign(function->identifier, function);
}

auto DzClass::get(DzString* identifier) -> DzFunction* {
  auto pos = functions.find(identifier);
  return pos != functions.end()
    ? pos->second
    : nullptr;
}
