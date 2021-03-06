#include "dzlist.h"

#include <sh/fmt.h>

DzList::DzList()
  : DzObject(Type::List) {}

DzList::operator bool() const {
  return size() > 0;
}

auto DzList::operator==(const DzList& other) const -> bool {
  return values == other.values;
}

auto DzList::operator[](std::size_t index) -> DzValue& {
  return values[index];
}

auto DzList::operator[](std::size_t index) const -> const DzValue& {
  return values[index];
}

auto DzList::repr() const -> std::string {
  return fmt::format("[{}]", fmt::join(values, ", "));
}

auto DzList::size() const -> std::size_t {
  return values.size();
}

DzListIterator::DzListIterator(DzObject* iteree)
  : DzIterator(iteree, "list"), index(0) {}

auto DzListIterator::done() const -> bool {
  return index >= iteree->as<DzList>()->size();
}

void DzListIterator::advance() {
  index++;
}

auto DzListIterator::current(Gc&) const -> DzValue {
  return (*iteree->as<DzList>())[index];
}

DzListReverseIterator::DzListReverseIterator(DzObject* iteree)
  : DzIterator(iteree, "list reverse"), index(iteree->as<DzList>()->size() - 1) {}

auto DzListReverseIterator::done() const -> bool {
  return index >= iteree->as<DzList>()->size();
}

void DzListReverseIterator::advance() {
  index--;
}

auto DzListReverseIterator::current(Gc&) const -> DzValue {
  return (*iteree->as<DzList>())[index];
}
