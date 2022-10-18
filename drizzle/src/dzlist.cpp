#include "dzlist.h"

#include "dziterator.h"
#include "gc.h"
#include "vm.h"

DzList::DzList()
  : DzObject(Type::List) {}

DzList::operator bool() const {
  return size() > 0;
}

auto DzList::operator==(const DzObject& other) const -> bool {
  return other.type == Type::List && other.as<DzList>()->values == values;
}

auto DzList::operator[](std::size_t index) -> DzValue& {
  return values[index];
}

auto DzList::operator[](std::size_t index) const -> const DzValue& {
  return values[index];
}

auto DzList::size() const -> std::size_t {
  return values.size();
}

auto DzList::repr() const -> std::string {
  return fmt::format("[{}]", fmt::join(values, ", "));
}

auto DzList::makeIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceIterator>(this);
}

auto DzList::makeReverseIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceReverseIterator>(this);
}

auto DzList::getAt(Vm& vm, std::size_t index) -> DzValue {
  return values[index];
}

auto DzList::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return refExpr(vm, expr);
}

void DzList::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  refExpr(vm, expr) = value;
}

auto DzList::refExpr(Vm& vm, const DzValue& expr) -> DzValue& {
  vm.expect(expr, DzValue::Type::Int);
  auto index = expr.i;
  if (index < 0) {
    index += values.size();
  }
  if (index < 0 || index >= values.size()) {
    vm.raise("list index out of range");
  }
  return values[index];
}
