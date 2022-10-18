#include "vm.h"

#include "dzmap.h"
#include "dznull.h"
#include "gc.h"

void Vm::defineMapMembers() {
  const auto members = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("clear"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto map = vm.stack.top().o->as<DzMap>();
        map->values.clear();
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("get"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto key = vm.stack.pop_value();
        const auto map = vm.stack.top().o->as<DzMap>();
        return map->get(key).value_or(&null);
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("set"), Arity::equal(2), [](Vm& vm, std::size_t) {
        const auto value = vm.stack.pop_value();
        const auto key = vm.stack.pop_value();
        const auto map = vm.stack.top().o->as<DzMap>();
        map->set(key, value);
        return &null;
      }
    ),
    gc.construct<DzFunction>(
      gc.construct<DzString>("has"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto key = vm.stack.pop_value();
        const auto map = vm.stack.top().o->as<DzMap>();
        return map->get(key).has_value();
      }
    ),
  };

  for (const auto& member : members) {
    this->members[int(DzObject::Type::Map)].insert_or_assign(member->identifier, member);
  }
}
