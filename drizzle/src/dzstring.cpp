#include "dzstring.h"

#include "dziterator.h"
#include "gc.h"
#include "vm.h"

auto DzString::Hash::operator()(const DzString* string) const -> std::size_t {
  return string->data_hash;
}

auto DzString::Equal::operator()(const DzString* a, const DzString* b) const -> bool {
  return a == b;
}

auto DzString::EqualData::operator()(const DzString* a, const DzString* b) const -> bool {
  return a->data == b->data;
}

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

auto DzString::in(Vm& vm, const DzValue& value) -> bool {
  vm.expect(value, DzObject::Type::String);
  return data.find(value->as<DzString>()->data) != std::string::npos;
}

auto DzString::getItem(Vm& vm, std::size_t index) -> DzValue {
  return vm.gc.construct<DzString>(std::string_view(
    data.begin() + index,
    data.begin() + index + 1
  ));
}

auto DzString::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return getItem(vm, toIndex(vm, expr));
}

auto DzString::toIndex(Vm& vm, const DzValue& expr) const -> std::size_t {
  vm.expect(expr, DzValue::Type::Int);
  const auto index = expr.i < 0
    ? expr.i + size()
    : expr.i;
  if (index < 0 || index >= size()) {
    vm.raise("string index out of range");
  }
  return index;
}
