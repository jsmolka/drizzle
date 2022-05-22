#include "vm.h"

#include <chrono>

#include "dzfunction.h"
#include "dznull.h"
#include "gc.h"
#include "vm.h"

void Vm::defineNativeFunctions() {
  const auto functions = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("assert"), Arity::equal(1), [](Vm& vm, std::size_t) {
        if (!vm.stack.pop_value()) {
          vm.raise("assertion failed");
        }
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("print"), Arity::greaterEqual(1), [](Vm& vm, std::size_t argc) {
        fmt::print("{}\n", fmt::join(vm.stack.end() - argc, vm.stack.end(), " "));
        vm.stack.pop(argc);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("repr"), Arity::equal(1), [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().repr());
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("time"), Arity::equal(0), [](Vm& vm, std::size_t) {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("type"), Arity::equal(1), [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().kind());
      }
    ),
  };

  const auto main = frames[0].function;
  for (const auto& function : functions) {
    const auto iter = main->identifiers.find(function->identifier);
    if (iter != main->identifiers.end()) {
      const auto& [identifier, index] = *iter;
      globals[index] = function;
    }
  }
}
