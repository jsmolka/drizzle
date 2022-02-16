#include "dzbuiltin.h"

#include <chrono>

#include <sh/fmt.h>

#include "vm.h"

DzBuiltIn::DzBuiltIn(const std::string& identifier, std::optional<std::size_t> arity, const Callback& callback)
  : DzObject(Type::BuiltIn), identifier(identifier), arity(arity), callback(callback) {}

auto DzBuiltIn::all() -> BuiltIns& {
  static BuiltIns builtins = {

    DzBuiltIn("assert", 1, [](Vm& vm, std::size_t) {
      if (!vm.stack.pop_value()) {
        vm.raise<RuntimeError>("assertion failed");
      }
      vm.stack.top() = {};
    }),

    DzBuiltIn("print", std::nullopt, [](Vm& vm, std::size_t argc) {
      fmt::print("{}\n", fmt::join(vm.stack.end() - argc, vm.stack.end(), " "));
      vm.stack.pop(argc);
      vm.stack.top() = {};
    }),

    DzBuiltIn("time", 0, [](Vm& vm, std::size_t) {
      using namespace std::chrono;
      vm.stack.top() = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }),

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
