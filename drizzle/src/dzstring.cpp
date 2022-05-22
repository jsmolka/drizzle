#include "dzstring.h"

DzString::DzString(const char* data)
  : DzString(std::string_view(data)) {}

DzString::DzString(std::string_view data)
  : DzString(std::string(data)) {}

DzString::DzString(const std::string& data)
  : DzObject(Type::String), data(data), hash(std::hash<std::string>{}(data)) {}

DzString::operator bool() const {
  return !data.empty();
}

auto DzString::repr() const -> std::string {
  return data;
}
