#include "vm.h"

#include <sh/ranges.h>

#include "dzlist.h"
#include "gc.h"

void Vm::defineListMembers() {
  const auto members = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("size"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        const auto list = vm.stack.peek(0).as<DzList>();
        return list->values.size();
      }
    ),
    // clear()
    gc.construct<DzFunction>(
      gc.construct<DzString>("push"), Arity::greaterEqual(1), [](Vm& vm, std::size_t argc) -> dzint {
        const auto list = vm.stack.peek(argc).as<DzList>();
        for (const auto& value : sh::range(vm.stack.end() - argc, vm.stack.end())) {
          list->values.push_back(value);
        }
        vm.stack.pop(argc);
        return list->values.size();
      }
    ),
    // insert(i, v)
    gc.construct<DzFunction>(
      gc.construct<DzString>("pop"), Arity::equal(0), [this](Vm& vm, std::size_t) {
        const auto list = vm.stack.peek(0).as<DzList>();
        if (list->values.empty()) {
          raise("cannot pop from empty list");
        }
        return list->values.pop_back_value();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("index"), Arity::equal(1), [this](Vm& vm, std::size_t) -> dzint {
        const auto list = vm.stack.peek(1).as<DzList>();
        const auto find = vm.stack.pop_value();
        for (const auto [index, value] : sh::enumerate(list->values)) {
          if (value == find) {  // Todo: proper value compare
            return index;
          }
        }
        return -1;
      }
    ),
  };

  for (const auto& member : members) {
    this->members[int(DzObject::Type::List)].insert_or_assign(member->identifier, member);
  }
}

