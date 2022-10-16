#include "dzbytes.h"

#include <sh/fmt.h>

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

auto DzBytes::repr() const -> std::string {
  return fmt::format("bytes({})", fmt::join(data, ", "));
}

auto DzBytes::size() const -> std::size_t {
  return data.size();
}

auto DzBytes::subscriptGet(Vm& vm, const DzValue& expr) -> DzValue {
  return static_cast<dzint>(subscript(vm, expr));
}

void DzBytes::subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value) {
  vm.expect(value, DzValue::Type::Int);
  subscript(vm, expr) = static_cast<u8>(value.i);
}

auto DzBytes::subscript(Vm& vm, const DzValue& expr) -> u8& {
  vm.expect(expr, DzValue::Type::Int);
  auto index = expr.i;
  if (index < 0) {
    index += size();
  }
  if (index < 0 || index >= size()) {
    vm.raise("bytes index out of range");
  }
  return data[index];
}

DzBytesIterator::DzBytesIterator(DzObject* iteree)
  : DzIterator(iteree, "bytes"), index(0) {}

auto DzBytesIterator::done() const -> bool {
  return index >= iteree->as<DzBytes>()->size();
}

void DzBytesIterator::advance() {
  index++;
}

auto DzBytesIterator::current(Gc&) const -> DzValue {
  return static_cast<dzint>((*iteree->as<DzBytes>())[index]);
}

DzBytesReverseIterator::DzBytesReverseIterator(DzObject* iteree)
  : DzIterator(iteree, "bytes reverse"), index(iteree->as<DzBytes>()->size() - 1) {}

auto DzBytesReverseIterator::done() const -> bool {
  return index >= iteree->as<DzBytes>()->size();
}

void DzBytesReverseIterator::advance() {
  index--;
}

auto DzBytesReverseIterator::current(Gc&) const -> DzValue {
  return static_cast<dzint>((*iteree->as<DzBytes>())[index]);
}
