#include "vm.h"

#include <sh/ranges.h>

#include "dzlist.h"
#include "dznull.h"
#include "gc.h"

void Vm::defineListMembers() {
  const auto members = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("size"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        return vm.stack.top().o->as<DzList>()->size();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("clear"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto list = vm.stack.top().o->as<DzList>();
        list->values.clear();
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("resize"), Arity::equal(2), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        const auto init = vm.stack.pop_value();
        const auto size = vm.stack.pop_value().i;
        const auto list = vm.stack.top().o->as<DzList>();
        if (size < 0) {
          vm.raise("negative resize size");
        }
        list->values.resize(size, init);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("push"), Arity::greaterEqual(1), [](Vm& vm, std::size_t argc) -> dzint {
        const auto list = vm.stack.peek(argc).o->as<DzList>();
        for (const auto& value : sh::range(vm.stack.end() - argc, vm.stack.end())) {
          list->values.push_back(value);
        }
        vm.stack.pop(argc);
        return list->size();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("insert"), Arity::equal(2), [](Vm& vm, std::size_t) -> dzint {
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        const auto value = vm.stack.pop_value();
        auto index = vm.stack.pop_value().i;
        const auto list = vm.stack.top().o->as<DzList>();
        if (index < 0) {
          index += list->size();
        }
        if (index < 0 || index > list->size()) {
          vm.raise("insert index out of range");
        }
        list->values.insert(list->values.begin() + index, value);
        return list->size();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("pop"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto list = vm.stack.top().o->as<DzList>();
        if (list->values.empty()) {
          vm.raise("pop from empty list");
        }
        return list->values.pop_back_value();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("remove"), Arity::equal(1), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        auto index = vm.stack.pop_value().i;
        const auto list = vm.stack.top().o->as<DzList>();
        if (index < 0) {
          index += list->size();
        }
        if (index < 0 || index >= list->size()) {
          vm.raise("remove index out of range");
        }
        const auto value = list->values[index];
        list->values.erase(list->values.begin() + index);
        return value;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("index"), Arity::equal(1), [](Vm& vm, std::size_t) -> dzint {
        const auto find = vm.stack.pop_value();
        const auto list = vm.stack.top().o->as<DzList>();
        for (const auto [index, value] : sh::enumerate(list->values)) {
          if (value == find) {
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

