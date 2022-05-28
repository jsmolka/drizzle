#include "dzstring.h"

#include <sh/fmt.h>

#include "gc.h"

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
  return fmt::format(R"("{}")", data);
}

DzStringIterator::DzStringIterator(DzObject* iteree)
  : DzIterator("string", static_cast<DzString*>(iteree)->data.size() ? iteree : nullptr) {}

void DzStringIterator::advance() {
  if (++index >= as<DzString>().data.size()) {
    iteree = nullptr;
  }
}

auto DzStringIterator::dereference(Gc& gc) const -> DzValue {
  return gc.construct<DzString>(std::string_view(
    as<DzString>().data.begin() + index,
    as<DzString>().data.begin() + index + 1
  ));
}
