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
  : DzIterator(static_cast<DzString*>(iteree)->data.size() ? iteree : nullptr) {}

void DzStringIterator::advance() {
  if (++index >= string()->data.size()) {
    iteree = nullptr;
  }
}

auto DzStringIterator::dereference(Gc& gc) const -> DzValue {
  return gc.construct<DzString>(std::string_view(
    string()->data.begin() + index,
    string()->data.begin() + index + 1
    ));
}

auto DzStringIterator::string() const -> const DzString* {
  assert(iteree);
  return static_cast<const DzString*>(iteree);
}
