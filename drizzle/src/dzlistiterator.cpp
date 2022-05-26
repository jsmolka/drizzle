#include "dzlistiterator.h"

#include "dzlist.h"

DzListIterator::operator bool() const {
  return index < static_cast<const DzList*>(iteree)->values.size();
}

void DzListIterator::increment() {
  ++index;
}

auto DzListIterator::dereference(Gc&) const -> DzValue {
  return static_cast<const DzList*>(iteree)->values[index];
}
