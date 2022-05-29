#include "dzmap.h"

#include <sh/fmt.h>

DzMap::DzMap()
  : DzObject(Type::Map) {}

DzMap::operator bool() const {
  return true;
}

auto DzMap::operator==(const DzMap& other) const -> bool {
  return values == other.values;
}

auto DzMap::repr() const -> std::string {
  return "maplmao";
}

auto DzMap::get(DzString* identifier) -> std::optional<DzValue> {
  const auto iter = values.find(identifier);
  return iter != values.end()
    ? iter->second
    : std::optional<DzValue>();
}

void DzMap::set(DzString* identifier, const DzValue& value) {
  values.insert_or_assign(identifier, value);
}
