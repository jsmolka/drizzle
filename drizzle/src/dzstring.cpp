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
  : DzIterator(iteree, "string") {
  set(0);
}

void DzStringIterator::advance() {
  set(index + 1);
}

auto DzStringIterator::dereference(Gc& gc) const -> DzValue {
  return gc.construct<DzString>((*iteree->as<DzString>())[index]);
}

void DzStringIterator::set(std::size_t value) {
  index = value;
  if (index >= iteree->as<DzString>()->size()) {
    iteree = nullptr;
  }
}

DzStringReverseIterator::DzStringReverseIterator(DzObject* iteree)
  : DzReverseIterator(iteree, "string") {
  set(iteree->as<DzString>()->size() - 1);
}

void DzStringReverseIterator::advance() {
  set(index - 1);
}

auto DzStringReverseIterator::dereference(Gc& gc) const -> DzValue {
  return gc.construct<DzString>((*iteree->as<DzString>())[index]);
}

void DzStringReverseIterator::set(std::size_t value) {
  index = value;
  if (index == -1 || index >= iteree->as<DzString>()->size()) {
    iteree = nullptr;
  }
}
