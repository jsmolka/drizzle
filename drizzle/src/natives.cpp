#include "vm.h"

#include <chrono>

#include "dzfunction.h"
#include "dznull.h"
#include "gc.h"

struct DzValuePrint : DzValue {};

template<>
struct fmt::formatter<DzValuePrint> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const DzValuePrint& value, FormatContext& ctx) const {
    return fmt::formatter<std::string>::format(
      value.is(DzObject::Type::String)
        ? value.as<DzString>()->data
        : value.repr(), ctx);
  }
};

void Vm::defineNatives() {
  const auto natives = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("assert"), Arity::equal(1), [](Vm& vm, std::size_t argc) {
        if (!vm.stack.pop_value()) {
          vm.raise("assertion failed");
        }
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("forward"), Arity::equal(1), [](Vm& vm, std::size_t) {
        return vm.forward(vm.stack.pop_value());
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("print"), Arity::greaterEqual(1), [](Vm& vm, std::size_t argc) {
        fmt::print("{}\n", fmt::join(
          static_cast<DzValuePrint*>(vm.stack.end()) - argc,
          static_cast<DzValuePrint*>(vm.stack.end()), " "));
        vm.stack.pop(argc);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("time"), Arity::equal(0), [](Vm& vm, std::size_t argc) {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("type"), Arity::equal(1), [](Vm& vm, std::size_t argc) {
        return vm.gc.construct<DzString>(vm.stack.pop_value().kind());
      }
    ),
  };

  const auto main = frames[0].function;
  for (const auto& native : natives) {
    const auto iter = main->identifiers.find(native->identifier);
    if (iter != main->identifiers.end()) {
      const auto& [identifier, index] = *iter;
      globals[index] = native;
    }
  }
}
