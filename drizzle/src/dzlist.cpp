#include "dzlist.h"

#include <sh/fmt.h>

DzList::DzList()
  : DzObject(Type::List) {}

DzList::operator bool() const {
  return !values.empty();
}

auto DzList::operator==(const DzList& other) const -> bool {
  return values == other.values;
}

auto DzList::repr() const -> std::string {
  return fmt::format("[{}]", fmt::join(values, ", "));
}

DzListIterator::DzListIterator(DzObject* iteree)
  : DzIterator(static_cast<DzList*>(iteree)->values.size() ? iteree : nullptr) {}

void DzListIterator::increment() {
  if (++index >= list()->values.size()) {
    iteree = nullptr;
  }
}

auto DzListIterator::dereference(Gc&) const -> DzValue {
  return list()->values[index];
}

auto DzListIterator::list() const -> const DzList* {
  assert(iteree);
  return static_cast<const DzList*>(iteree);
}
