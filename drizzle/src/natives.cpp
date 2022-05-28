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

auto Vm::nativeAssert() -> DzValue {
  if (!stack.pop_value()) {
    raise("assertion failed");
  }
  return &null;
}

auto Vm::nativePrint(std::size_t argc) -> DzValue {
  fmt::print("{}\n", fmt::join(
    static_cast<DzValuePrint*>(stack.end()) - argc,
    static_cast<DzValuePrint*>(stack.end()), " "));
  stack.pop(argc);
  return &null;
}

auto Vm::nativeTime() -> DzValue {
  using namespace std::chrono;
  return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

auto Vm::nativeType() -> DzValue {
  return gc.construct<DzString>(stack.pop_value().kind());
}

void Vm::defineNatives() {
  const auto natives = {
    gc.construct<DzFunction>(gc.construct<DzString>("assert"), Arity::equal(1),        [](Vm& vm, std::size_t argc) { return vm.nativeAssert();    }),
    gc.construct<DzFunction>(gc.construct<DzString>("print"),  Arity::greaterEqual(1), [](Vm& vm, std::size_t argc) { return vm.nativePrint(argc); }),
    gc.construct<DzFunction>(gc.construct<DzString>("time"),   Arity::equal(0),        [](Vm& vm, std::size_t argc) { return vm.nativeTime();      }),
    gc.construct<DzFunction>(gc.construct<DzString>("type"),   Arity::equal(1),        [](Vm& vm, std::size_t argc) { return vm.nativeType();      }),
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
