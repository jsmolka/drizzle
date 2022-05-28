#include "dziterator.h"

#include <sh/fmt.h>

DzIterator::DzIterator(std::string_view name, DzObject* iteree)
  : DzObject(Type::Iterator), iteree(iteree), name(name) {}

DzIterator::operator bool() const {
  return iteree;
}

auto DzIterator::repr() const -> std::string {
  return fmt::format("<{} iterator>", name);
}
