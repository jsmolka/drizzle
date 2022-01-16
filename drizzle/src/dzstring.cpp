#include "dzstring.h"

DzString::DzString(std::string&& data, std::size_t hash)
  : DzObject(Type::String), data(std::move(data)), hash(hash) {}

DzString::operator bool() const {
  return !data.empty();
}

auto DzString::repr() const -> std::string {
    return data;
}

auto DzString::name() const -> std::string_view {
  return "string";
}
