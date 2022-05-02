#include "dznativefunction.h"

DzNativeFunction::DzNativeFunction(std::string_view identifier, std::optional<std::size_t> arity, const Callback& callback)
  : DzObject(Type::NativeFunction), identifier(identifier), arity(arity), callback(callback) {}

DzNativeFunction::operator bool() const {
  return true;
}

auto DzNativeFunction::repr() const -> std::string {
  return fmt::format("<function {}>", identifier);
}

auto DzNativeFunction::name() const -> std::string_view {
  return "function";
}
