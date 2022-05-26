#include "dzstringiterator.h"

#include "dzstring.h"
#include "gc.h"

DzStringIterator::operator bool() const {
  return index < static_cast<const DzString*>(iteree)->data.size();
}

void DzStringIterator::increment() {
  ++index;
}

auto DzStringIterator::dereference(Gc& gc) const -> DzValue {
  const auto string = static_cast<const DzString*>(iteree);
  return gc.construct<DzString>(std::string_view(
    string->data.begin() + index,
    string->data.begin() + index + 1
  ));
}
