#include "vm.h"

#include <sh/ranges.h>

#include "dzbytes.h"
#include "dznull.h"
#include "gc.h"

void Vm::defineBytesMembers() {
  const auto members = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("size"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        return vm.stack.top().o->as<DzBytes>()->size();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("clear"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto bytes = vm.stack.top().o->as<DzBytes>();
        bytes->data.clear();
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("resize"), Arity::equal(2), [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        const auto init  = vm.stack.pop_value().i;
        const auto size  = vm.stack.pop_value().i;
        const auto bytes = vm.stack.top().o->as<DzBytes>();
        if (size < 0) {
          vm.raise("negative resize size");
        }
        bytes->data.resize(size, init);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("push"), Arity::greaterEqual(1), [](Vm& vm, std::size_t argc) -> dzint {
        const auto bytes = vm.stack.peek(argc).o->as<DzBytes>();
        for (const auto& value : sh::range(vm.stack.end() - argc, vm.stack.end())) {
          vm.expect(value, DzValue::Type::Int);
          bytes->data.push_back(value.i);
        }
        vm.stack.pop(argc);
        return bytes->size();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("insert"), Arity::equal(2), [](Vm& vm, std::size_t) -> dzint {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        const auto value = vm.stack.pop_value().i;
        auto index = vm.stack.pop_value().i;
        const auto bytes = vm.stack.top().o->as<DzBytes>();
        if (index < 0) {
          index += bytes->size();
        }
        if (index < 0 || index > bytes->size()) {
          vm.raise("insert index out of range");
        }
        bytes->data.insert(bytes->data.begin() + index, value);
        return bytes->size();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("pop"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        const auto bytes = vm.stack.top().o->as<DzBytes>();
        if (bytes->data.empty()) {
          vm.raise("pop from empty bytes");
        }
        return bytes->data.pop_back_value();
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("remove"), Arity::equal(1), [](Vm& vm, std::size_t) -> dzint {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        auto index = vm.stack.pop_value().i;
        const auto bytes = vm.stack.top().o->as<DzBytes>();
        if (index < 0) {
          index += bytes->size();
        }
        if (index < 0 || index >= bytes->size()) {
          vm.raise("remove index out of range");
        }
        const auto value = bytes->data[index];
        bytes->data.erase(bytes->data.begin() + index);
        return value;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("index"), Arity::equal(1), [](Vm& vm, std::size_t) -> dzint {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        const auto find  = vm.stack.pop_value().i;
        const auto bytes = vm.stack.top().o->as<DzBytes>();
        for (const auto [index, value] : sh::enumerate(bytes->data)) {
          if (value == find) {
            return index;
          }
        }
        return -1;
      }
    ),
  };

  for (const auto& member : members) {
    this->members[int(DzObject::Type::Bytes)].insert_or_assign(member->identifier, member);
  }
}
