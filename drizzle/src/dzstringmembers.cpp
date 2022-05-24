#include "vm.h"

#include <sh/ranges.h>

#include "dzstring.h"
#include "gc.h"

void Vm::defineStringMembers() {
  const auto members = {
    gc.construct<DzFunction>(
      gc.construct<DzString>("size"), Arity::equal(0), [](Vm& vm, std::size_t) -> dzint {
        const auto string = vm.stack.top().as<DzString>();
        return string->data.size();
      }
    ),
  };

  for (const auto& member : members) {
    this->members[int(DzObject::Type::String)].insert_or_assign(member->identifier, member);
  }
}

