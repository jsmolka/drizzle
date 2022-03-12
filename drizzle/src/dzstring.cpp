#include "dzstring.h"

DzString::DzString(const std::string& data)
  : DzObject(Type::String), data(data), hash(std::hash<std::string>{}(data)) {}

DzString::operator bool() const {
  return !data.empty();
}

auto DzString::repr() const -> std::string {
    return data;
}

auto DzString::name() const -> std::string_view {
  return "string";
}
