#include "dzfunction.h"

DzFunction::DzFunction()
  : DzObject(Type::Function), identifier(nullptr) {}

DzFunction::operator bool() const {
  return true;
}

auto DzFunction::repr() const -> std::string {
  return fmt::format("<function {}>", identifier->repr());
}

auto DzFunction::name() const -> std::string_view {
  return "function";
}
