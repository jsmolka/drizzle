#include "dzstringiterator.h"

#include "gc.h"

DzStringIterator::DzStringIterator(DzObject* iteree)
  : DzIterator(static_cast<DzString*>(iteree)->data.size() ? iteree : nullptr) {}

void DzStringIterator::increment() {
  if (++index >= string()->data.size()) {
    iteree = nullptr;
  }
}

auto DzStringIterator::dereference(Gc& gc) const -> DzValue {
  return gc.construct<DzString>(std::string_view(
    string()->data.begin() + index,
    string()->data.begin() + index + 1
  ));
}

auto DzStringIterator::string() const -> const DzString* {
  assert(iteree);
  return static_cast<const DzString*>(iteree);
}
