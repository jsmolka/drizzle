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
