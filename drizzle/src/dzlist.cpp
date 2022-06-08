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
  : DzIterator(iteree, "list") {
  set(0);
}

void DzListIterator::advance() {
  set(index + 1);
}

auto DzListIterator::current(Gc&) const -> DzValue {
  return (*iteree->as<DzList>())[index];
}

void DzListIterator::set(std::size_t value) {
  index = value;
  if (index >= iteree->as<DzList>()->size()) {
    iteree = nullptr;
  }
}

DzListReverseIterator::DzListReverseIterator(DzObject* iteree)
  : DzIterator(iteree, "list reverse") {
  set(iteree->as<DzList>()->size() - 1);
}

void DzListReverseIterator::advance() {
  set(index - 1);
}

auto DzListReverseIterator::current(Gc&) const -> DzValue {
  return (*iteree->as<DzList>())[index];
}

void DzListReverseIterator::set(std::size_t value) {
  index = value;
  if (index == -1 || index >= iteree->as<DzList>()->size()) {
    iteree = nullptr;
  }
}
