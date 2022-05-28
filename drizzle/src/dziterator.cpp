#include "dziterator.h"

#include <sh/fmt.h>

DzIterator::DzIterator(DzObject* iteree)
  : DzObject(Type::Iterator), iteree(iteree) {}

DzIterator::operator bool() const {
  return iteree;
}

auto DzIterator::repr() const -> std::string {
  return fmt::format("<{} iterator>", iteree->type);
}
