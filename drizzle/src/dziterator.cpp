#include "dziterator.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzIterator::DzIterator(DzObject* iteree, std::string_view type)
  : DzObject(Type::Iterator), iteree(iteree), type(type) {}

DzIterator::operator bool() const {
  if (iteree && done()) {
    iteree = nullptr;
  }
  return iteree;
}

auto DzIterator::repr() const -> std::string {
  return fmt::format("<{} iterator at 0x{:016X}>", type, sh::cast<u64>(this));
}
