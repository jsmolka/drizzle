#include "dzfunction.h"

DzFunction::DzFunction()
  : DzFunction(nullptr, std::nullopt) {}

DzFunction::DzFunction(DzString* identifier, Arity arity)
  : DzObject(Type::Function), identifier(identifier), arity(arity) {}

DzFunction::operator bool() const {
  return true;
}

auto DzFunction::repr() const -> std::string {
  return fmt::format("<function {}>", identifier->repr());
}

auto DzFunction::name() const -> std::string_view {
  return "function";
}
