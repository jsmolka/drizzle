#include "dziterator.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzIterator::DzIterator(std::string_view name, DzObject* iteree)
  : DzObject(Type::Iterator), iteree(iteree), name(name) {}

DzIterator::operator bool() const {
  return iteree;
}

auto DzIterator::repr() const -> std::string {
  return fmt::format("<{} iterator at 0x{:016X}>", name, sh::cast<u64>(this));
}
