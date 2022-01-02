#include "dzstring.h"

DzString::DzString(std::string&& data, std::size_t hash)
  : DzObject(Type::String), data(std::move(data)), hash(hash) {}

DzString::operator bool() const {
  return !data.empty();
}
