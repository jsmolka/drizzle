#include "dzlistiterator.h"

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
