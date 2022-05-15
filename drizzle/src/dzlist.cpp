#include "dzlist.h"

#include <sh/fmt.h>

DzList::DzList(DzClass* class_)
  : DzObject(Type::List), class_(class_) {}

DzList::operator bool() const {
  return !values.empty();
}

auto DzList::kind() const -> std::string_view {
  return "list";
}

auto DzList::repr() const -> std::string {
  return fmt::format("[{}]", fmt::join(values, ", "));
}
