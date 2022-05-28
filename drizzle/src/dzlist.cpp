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
  : DzIterator("list", static_cast<DzList*>(iteree)->values.size() ? iteree : nullptr) {}

void DzListIterator::advance() {
  if (++index >= as<DzList>().values.size()) {
    iteree = nullptr;
  }
}

auto DzListIterator::dereference(Gc&) const -> DzValue {
  return as<DzList>().values[index];
}

DzListReverseIterator::DzListReverseIterator(DzObject* iteree)
  : DzReverseIterator("list", static_cast<DzList*>(iteree)->values.size() ? iteree : nullptr)
  , index(static_cast<DzList*>(iteree)->values.size() - 1) {}

void DzListReverseIterator::advance() {
  if (index-- == 0) {
    iteree = nullptr;
  }
}

auto DzListReverseIterator::dereference(Gc&) const -> DzValue {
  return as<DzList>().values[index];
}
