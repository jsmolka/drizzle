#include "compiler.h"

#include <chrono>

#include "dznativefunction.h"
#include "dznull.h"
#include "gc.h"
#include "vm.h"

void Compiler::defineNativeFunctions() {
  const auto functions = {
    gc.construct<DzNativeFunction>(
      "assert", 1, [](Vm& vm, std::size_t) {
        if (!vm.stack.pop_value()) {
          vm.raise("assertion failed");
        }
        return &null;
      }
    ),
    gc.construct<DzNativeFunction>(
      "print", std::nullopt, [](Vm& vm, std::size_t argc) {
        fmt::print("{}\n", fmt::join(vm.stack.end() - argc, vm.stack.end(), " "));
        vm.stack.pop(argc);
        return &null;
      }
    ),
    gc.construct<DzNativeFunction>(
      "repr", 1, [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().repr());
      }
    ),
    gc.construct<DzNativeFunction>(
      "time", 0, [](Vm& vm, std::size_t) {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
      }
    ),
    gc.construct<DzNativeFunction>(
      "type", 1, [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().name());
      }
    ),
  };

  for (const auto& function : functions) {
    define(function->identifier);
    emitConstant(function);
  }
}
