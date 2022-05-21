#include "vm.h"

#include <chrono>

#include "dzfunction.h"
#include "dzlist.h"
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

void Vm::defineNativeMembersList() {
  const auto functions = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("size"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto list = vm.stack.peek(0).as<DzList>();
        return static_cast<dzint>(list->values.size());
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("push"), Arity::equal(1), [](Vm& vm, std::size_t) {
        // Todo: support args >= 1
        const auto list = vm.stack.peek(1).as<DzList>();
        list->values.push_back(vm.stack.pop_value());
        return static_cast<dzint>(list->values.size());
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("pop"), Arity::equal(0), [this](Vm& vm, std::size_t) {
        const auto list = vm.stack.peek(0).as<DzList>();
        if (list->values.empty()) {
          raise("cannot pop from empty list");
        }
        const auto value = list->values.back();
        list->values.pop_back();
        return value;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("contains"), Arity::equal(1), [this](Vm& vm, std::size_t) {
        const auto list = vm.stack.peek(1).as<DzList>();
        const auto find = vm.stack.pop_value();
        for (const auto& value : list->values) {
          if (value == find) {
            return true;
          }
        }
        return false;
      }
    ),
  };

  for (const auto& function : functions) {
    members[int(DzObject::Type::List)].insert_or_assign(function->identifier, function);
  }
}
