#include "dzbytes.h"

#include <sh/ranges.h>

#include "dznull.h"
#include "dziterator.h"
#include "gc.h"
#include "vm.h"

DzBytes::DzBytes()
  : DzObject(Type::Bytes) {}

DzBytes::DzBytes(const sh::vector<u8>& data)
  : DzObject(Type::Bytes), data(data) {}

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
  return sh::contains(data, static_cast<u8>(value.i));
}

auto DzBytes::getItem(Vm& vm, std::size_t index) -> DzValue {
  return static_cast<dzint>(data[index]);
}

auto DzBytes::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return getItem(vm, toIndex(vm, expr));
}

auto DzBytes::getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue {
  defineMembers(vm);
  return DzObject::getProp(vm, prop, bind);
}

void DzBytes::setItem(Vm& vm, std::size_t index, const DzValue& value) {
  vm.expect(value, DzValue::Type::Int);
  data[index] = static_cast<u8>(value.i);
}

void DzBytes::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  setItem(vm, toIndex(vm, expr), value);
}

void DzBytes::defineMembers(Vm& vm) {
  constexpr auto kSlot = int(Type::Bytes);
  if (vm.members[kSlot].size() > 0) {
    return;
  }

  const auto members = {
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("clear"), 0, [](Vm& vm, std::size_t) {
        const auto self = vm.stack.top()->as<DzBytes>();
        self->data.clear();
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("resize"), 2, [](Vm& vm, std::size_t) {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        vm.expect(vm.stack.peek(1), DzValue::Type::Int);
        const auto init = vm.stack.pop_value().i;
        const auto size = vm.stack.pop_value().i;
        const auto self = vm.stack.top()->as<DzBytes>();
        if (size < 0) {
          vm.raise("negative resize size");
        }
        self->data.resize(size, static_cast<u8>(init));
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("push"), 1, [](Vm& vm, std::size_t) -> dzint {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        const auto value = vm.stack.pop_value().i;
        const auto self  = vm.stack.top()->as<DzBytes>();
        self->data.push_back(static_cast<u8>(value));
        return self->size();
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("insert"), 2, [](Vm& vm, std::size_t) -> dzint {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        const auto value = vm.stack.pop_value().i;
        const auto index = vm.stack.pop_value();
        const auto self  = vm.stack.top()->as<DzBytes>();
        self->data.insert(self->data.begin() + self->toInsertIndex(vm, index), static_cast<u8>(value));
        return self->size();
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("pop"), 0, [](Vm& vm, std::size_t) -> dzint {
        const auto self = vm.stack.top()->as<DzBytes>();
        if (self->size() == 0) {
          vm.raise("pop from empty bytes");
        }
        return self->data.pop_back_value();
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("remove"), 1, [](Vm& vm, std::size_t) -> dzint {
        const auto self  = vm.stack.peek(1)->as<DzBytes>();
        const auto index = self->toIndex(vm, vm.stack.pop_value());
        const auto value = self->data[index];
        self->data.erase(self->data.begin() + index);
        return value;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("index"), 1, [](Vm& vm, std::size_t) -> dzint {
        vm.expect(vm.stack.peek(0), DzValue::Type::Int);
        const auto find = vm.stack.pop_value().i;
        const auto self = vm.stack.top()->as<DzBytes>();
        for (const auto [index, value] : sh::enumerate(self->data)) {
          if (value == static_cast<u8>(find)) {
            return index;
          }
        }
        return -1;
      }
    ),
  };

  for (const auto& member : members) {
    vm.members[kSlot].insert_or_assign(member->identifier, member);
  }
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

auto DzBytes::toInsertIndex(Vm& vm, const DzValue& expr) const -> std::size_t {
  vm.expect(expr, DzValue::Type::Int);
  const auto index = expr.i < 0
    ? expr.i + size()
    : expr.i;
  if (index < 0 || index > size()) {
    vm.raise("bytes index out of range");
  }
  return index;
}
