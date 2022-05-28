#include "dzreverseiterator.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzReverseIterator::DzReverseIterator(DzObject* iteree, std::string_view type)
  : DzObject(Type::ReverseIterator), iteree(iteree), type(type) {}

DzReverseIterator::operator bool() const {
  return iteree;
}

auto DzReverseIterator::repr() const -> std::string {
  return fmt::format("<{} reverse iterator at 0x{:016X}>", type, sh::cast<u64>(this));
}
