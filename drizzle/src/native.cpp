#include "vm.h"

#include <chrono>

#include "dzfunction.h"
#include "dznull.h"
#include "gc.h"
#include "vm.h"

void Vm::defineFunctions() {
  const auto functions = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("assert"), 1, [](Vm& vm, std::size_t) {
        if (!vm.stack.pop_value()) {
          vm.raise("assertion failed");
        }
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("print"), std::nullopt, [](Vm& vm, std::size_t argc) {
        fmt::print("{}\n", fmt::join(vm.stack.end() - argc, vm.stack.end(), " "));
        vm.stack.pop(argc);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("repr"), 1, [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().repr());
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("time"), 0, [](Vm& vm, std::size_t) {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("type"), 1, [](Vm& vm, std::size_t) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().kind());
      }
    ),
  };

  const auto main = frames[0].function;
  for (const auto& function : functions) {
    const auto iter = main->globals.find(function->identifier);
    if (iter != main->globals.end()) {
      const auto& [identifier, index] = *iter;
      if (index >= globals.size()) {
        globals.resize(index + 1);
      }
      globals[index] = function;
    }
  }
}
