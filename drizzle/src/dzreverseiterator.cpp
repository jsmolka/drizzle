#include "dzreverseiterator.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzReverseIterator::DzReverseIterator(std::string_view name, DzObject* iteree)
  : DzObject(Type::ReverseIterator), iteree(iteree), name(name) {}

DzReverseIterator::operator bool() const {
  return iteree;
}

auto DzReverseIterator::repr() const -> std::string {
  return fmt::format("<{} reverse iterator at 0x{:016X}>", name, sh::cast<u64>(this));
}
