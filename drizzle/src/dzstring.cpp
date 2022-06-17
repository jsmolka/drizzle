#include "dzstring.h"

#include <sh/fmt.h>

#include "gc.h"

DzString::DzString()
  : DzString(std::string{}) {}

DzString::DzString(const char* data)
  : DzString(std::string_view(data)) {}

DzString::DzString(std::string_view data)
  : DzString(std::string(data)) {}

DzString::DzString(const std::string& data)
  : DzObject(Type::String), data(data), hash(std::hash<std::string>{}(data)) {}

DzString::operator bool() const {
  return size() > 0;
}

auto DzString::operator[](std::size_t index) const -> std::string_view {
  return std::string_view(
    data.begin() + index,
    data.begin() + index + 1
  );
}

auto DzString::repr() const -> std::string {
  return fmt::format(R"("{}")", data);
}

auto DzString::size() const -> std::size_t {
  return data.size();
}

DzStringIterator::DzStringIterator(DzObject* iteree)
  : DzIterator(iteree, "string"), index(0) {}

auto DzStringIterator::done() const -> bool {
  return index >= iteree->as<DzString>()->size();
}

void DzStringIterator::advance() {
  index++;
}

auto DzStringIterator::current(Gc& gc) const -> DzValue {
  return gc.construct<DzString>((*iteree->as<DzString>())[index]);
}

DzStringReverseIterator::DzStringReverseIterator(DzObject* iteree)
  : DzIterator(iteree, "string reverse"), index(iteree->as<DzString>()->size() - 1) {}

auto DzStringReverseIterator::done() const -> bool {
  return index >= iteree->as<DzString>()->size();
}

void DzStringReverseIterator::advance() {
  index--;
}

auto DzStringReverseIterator::current(Gc& gc) const -> DzValue {
  return gc.construct<DzString>((*iteree->as<DzString>())[index]);
}
