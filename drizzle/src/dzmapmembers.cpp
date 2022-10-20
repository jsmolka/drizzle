#include "vm.h"

#include "dzmap.h"
#include "dznull.h"
#include "gc.h"

void Vm::defineMapMembers() {
  const auto members = {
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("clear"), Arity::equal(0), [](Vm& vm, std::size_t) {
        const auto map = vm.stack.top().o->as<DzMap>();
        map->values.clear();
        return &null;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("get"), Arity::equal(1), [](Vm& vm, std::size_t) {
        const auto key = vm.stack.pop_value();
        const auto map = vm.stack.top().o->as<DzMap>();
        return map->get(key).value_or(&null);
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("set"), Arity::equal(2), [](Vm& vm, std::size_t) {
        const auto value = vm.stack.pop_value();
        const auto key = vm.stack.pop_value();
        const auto map = vm.stack.top().o->as<DzMap>();
        map->set(key, value);
        return &null;
      }
    ),
    gc.constructNoCollect<DzFunction>(
      gc.constructNoCollect<DzString>("has"), Arity::equal(1), [](Vm& vm, std::size_t) {
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
