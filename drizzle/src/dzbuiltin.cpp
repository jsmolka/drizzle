#include "dzbuiltin.h"

#include <chrono>

#include "dznull.h"
#include "gc.h"
#include "vm.h"

DzBuiltIn::DzBuiltIn(std::string_view identifier, std::optional<std::size_t> arity, const Callback& callback)
  : DzObject(Type::BuiltIn), identifier(identifier), arity(arity), callback(callback) {}

DzBuiltIn::operator bool() const {
  return true;
}

auto DzBuiltIn::repr() const -> std::string {
  return fmt::format("<function {}>", identifier);
}

auto DzBuiltIn::name() const -> std::string_view {
  return "function";
}

std::vector<DzBuiltIn> DzBuiltIn::all = {
  {
    "assert", 1, [](Vm& vm, std::size_t) {
      if (!vm.stack.pop_value()) {
        vm.raise("assertion failed");
      }
      return &null;
    }
  },
  {
    "print", std::nullopt, [](Vm& vm, std::size_t argc) {
      fmt::print("{}\n", fmt::join(vm.stack.end() - argc, vm.stack.end(), " "));
      vm.stack.pop(argc);
      return &null;
    }
  },
  {
    "time", 0, [](Vm& vm, std::size_t) {
      using namespace std::chrono;
      return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }
  },
  {
    "type", 1, [](Vm& vm, std::size_t) {
      return vm.gc.construct<DzString>(vm.stack.pop_value().name());
    }
  },
  {
    "repr", 1, [](Vm& vm, std::size_t) {
      return vm.gc.construct<DzString>(vm.stack.pop_value().repr());
    }
  },
};
