#include "dzbytes.h"

#include <sh/fmt.h>
#include <sh/ranges.h>

#include "dziterator.h"
#include "gc.h"
#include "vm.h"

DzBytes::DzBytes()
  : DzObject(Type::Bytes) {}

DzBytes::operator bool() const {
  return size() > 0;
}

auto DzBytes::operator==(const DzObject& other) const -> bool {
  return other.type == Type::Bytes && other.as<DzBytes>()->data == data;
}

auto DzBytes::size() const -> std::size_t {
  return data.size();
}

auto DzBytes::repr() const -> std::string {
  return fmt::format("bytes({})", fmt::join(data, ", "));
}

auto DzBytes::makeIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceIterator>(this);
}

auto DzBytes::makeReverseIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceReverseIterator>(this);
}

auto DzBytes::in(Vm& vm, const DzValue& value) -> bool {
  vm.expect(value, DzValue::Type::Int);
  return sh::contains(data, value.i);
}

auto DzBytes::getItem(Vm& vm, std::size_t index) -> DzValue {
  return static_cast<dzint>(data[index]);
}

auto DzBytes::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return getItem(vm, toIndex(vm, expr));
}

void DzBytes::setItem(Vm& vm, std::size_t index, const DzValue& value) {
  vm.expect(value, DzValue::Type::Int);
  data[index] = static_cast<u8>(value.i);
}

void DzBytes::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  setItem(vm, toIndex(vm, expr), value);
}

auto DzBytes::toIndex(Vm& vm, const DzValue& expr) const -> std::size_t {
  vm.expect(expr, DzValue::Type::Int);
  const auto index = expr.i < 0
    ? expr.i + size()
    : expr.i;
  if (index < 0 || index >= size()) {
    vm.raise("bytes index out of range");
  }
  return index;
}
