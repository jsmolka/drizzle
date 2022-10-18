#include "dzbytes.h"

#include <sh/fmt.h>

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

auto DzBytes::operator[](std::size_t index) -> u8& {
  return data[index];
}

auto DzBytes::operator[](std::size_t index) const -> const u8& {
  return data[index];
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

auto DzBytes::getItem(Vm& vm, std::size_t index) -> DzValue {
  return static_cast<dzint>(data[index]);
}

auto DzBytes::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return static_cast<dzint>(refExpr(vm, expr));
}

void DzBytes::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  vm.expect(value, DzValue::Type::Int);
  refExpr(vm, expr) = static_cast<u8>(value.i);
}

auto DzBytes::refExpr(Vm& vm, const DzValue& expr) -> u8& {
  vm.expect(expr, DzValue::Type::Int);
  auto index = expr.i;
  if (index < 0) {
    index += data.size();
  }
  if (index < 0 || index >= data.size()) {
    vm.raise("bytes index out of range");
  }
  return data[index];
}
