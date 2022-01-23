#include "dzfunction.h"

DzFunction::DzFunction()
  : DzObject(Type::Function) {}

DzFunction::operator bool() const {
  return true;
}

auto DzFunction::repr() const -> std::string {
  return fmt::format("<function {}>", identifier);
}

auto DzFunction::name() const -> std::string_view {
  return "function";
}
