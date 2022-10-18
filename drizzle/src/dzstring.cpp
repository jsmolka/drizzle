#include "dzstring.h"

#include <sh/fmt.h>

#include "gc.h"
#include "vm.h"

DzString::DzString()
  : DzString(std::string{}) {}

DzString::DzString(const char* data)
  : DzString(std::string_view(data)) {}

DzString::DzString(std::string_view data)
  : DzString(std::string(data)) {}

DzString::DzString(const std::string& data)
  : DzObject(Type::String), data(data), data_hash(std::hash<std::string>{}(data)) {}

DzString::operator bool() const {
  return size() > 0;
}

auto DzString::operator[](std::size_t index) const -> std::string_view {
  return std::string_view(
    data.begin() + index,
    data.begin() + index + 1
  );
}

auto DzString::hash() const -> std::size_t {
  return data_hash;
}

auto DzString::size() const -> std::size_t {
  return data.size();
}

auto DzString::repr() const -> std::string {
  return fmt::format(R"("{}")", data);
}

auto DzString::makeIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceIterator>(this);
}

auto DzString::makeReverseIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceReverseIterator>(this);
}

auto DzString::getItem(Vm& vm, std::size_t index) -> DzValue {
  return vm.gc.construct<DzString>((*this)[index]);
}

auto DzString::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  vm.expect(expr, DzValue::Type::Int);
  auto index = expr.i;
  if (index < 0) {
    index += size();
  }
  if (index < 0 || index >= size()) {
    vm.raise("string index out of range");
  }
  return vm.gc.construct<DzString>((*this)[index]);
}
