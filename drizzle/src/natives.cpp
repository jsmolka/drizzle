#include "vm.h"

#include <chrono>
#include <sh/ranges.h>

#include "dzbytes.h"
#include "dzfunction.h"
#include "dznull.h"
#include "dzrange.h"
#include "gc.h"

struct DzValuePrint : DzValue {};

template<>
struct fmt::formatter<DzValuePrint> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const DzValuePrint& value, FormatContext& ctx) const {
    return fmt::formatter<std::string>::format(
      value.is(DzObject::Type::String)
        ? value.o->as<DzString>()->data
        : value.repr(), ctx);
  }
};

void Vm::defineNatives() {
  const auto natives = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("assert"), Arity::equal(1), [](Vm& vm, std::size_t) {
        if (!vm.stack.pop_value()) {
          vm.raise("assertion failed");
        }
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("bytes"), Arity::greaterEqual(0), [](Vm& vm, std::size_t argc) {
        const auto bytes = vm.gc.construct<DzBytes>();
        bytes->data.reserve(argc);
        for (const auto& value : sh::range(vm.stack.end() - argc, vm.stack.end())) {
          vm.expect(value, DzValue::Type::Int);
          bytes->data.push_back(value.i);
        }
        vm.stack.pop(argc);
        return bytes;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("forward"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto iterator = vm.forward(vm.stack.top());
        vm.stack.pop();
        return iterator;
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
      gc.construct<DzString>("range"), Arity::equal(3), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        vm.expect(vm.stack.peek(2), DzValue::Type::Int);
        const auto step  = vm.stack.pop_value().i;
        const auto stop  = vm.stack.pop_value().i;
        const auto start = vm.stack.pop_value().i;
        if (step == 0) {
          vm.raise("range() step cannot be zero");
        }
        return vm.gc.construct<DzRange>(start, stop, step);
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("reverse"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto iterator = vm.reverse(vm.stack.top());
        vm.stack.pop();
        return iterator;
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
  for (const auto& native : natives) {
    const auto iter = main->identifiers.find(native->identifier);
    if (iter != main->identifiers.end()) {
      const auto& [identifier, index] = *iter;
      globals[index] = native;
    }
  }
}
