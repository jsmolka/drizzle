#include "dzbuiltin.h"

#include <sh/fmt.h>

#include "vm.h"

DzBuiltIn::DzBuiltIn(const std::string& identifier, std::size_t arity, const Callback& callback)
  : DzObject(Type::BuiltIn), identifier(identifier), arity(arity), callback(callback) {}

auto DzBuiltIn::all() -> std::array<DzBuiltIn, 1>& {
  static std::array<DzBuiltIn, 1> builtins = {
    DzBuiltIn("assert", 1, [](Vm& vm) {
      if (!vm.stack.pop_value()) {
        vm.raise<RuntimeError>("assertion failed");
      }
    })
  };
  return builtins;
}

DzBuiltIn::operator bool() const {
  return true;
}

auto DzBuiltIn::repr() const -> std::string {
  return fmt::format("<function {}>", identifier);
}

auto DzBuiltIn::name() const -> std::string_view {
  return "function";
}
