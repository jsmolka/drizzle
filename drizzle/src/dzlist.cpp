#include "dzlist.h"

#include <sh/ranges.h>

#include "dziterator.h"
#include "dznull.h"
#include "gc.h"
#include "vm.h"

DzList::DzList()
  : DzObject(Type::List) {}

DzList::DzList(const sh::vector<DzValue>& values)
  : DzObject(Type::List), values(values) {}

DzList::operator bool() const {
  return size() > 0;
}

auto DzList::operator==(const DzObject& other) const -> bool {
  return other.type == Type::List && other.as<DzList>()->values == values;
}

auto DzList::size() const -> std::size_t {
  return values.size();
}

auto DzList::repr() const -> std::string {
  return fmt::format("[{}]", fmt::join(values, ", "));
}

void DzList::mark(Gc& gc) {
  DzObject::mark(gc);
  for (const auto& value : values) {
    gc.mark(value);
  }
}

auto DzList::makeIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceIterator>(this);
}

auto DzList::makeReverseIterator(Vm& vm) -> DzValue {
  return vm.gc.construct<DzSequenceReverseIterator>(this);
}

auto DzList::in(Vm& vm, const DzValue& value) -> bool {
  return sh::contains(values, value);
}

auto DzList::getItem(Vm& vm, std::size_t index) -> DzValue {
  return values[index];
}

auto DzList::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return getItem(vm, toIndex(vm, expr));
}

auto DzList::getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue {
  defineMembers(vm);
  return DzObject::getProp(vm, prop, bind);
}

void DzList::setItem(Vm& vm, std::size_t index, const DzValue& value) {
  values[index] = value;
}

void DzList::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  setItem(vm, toIndex(vm, expr), value);
}

void DzList::defineMembers(Vm& vm) {
  constexpr auto kSlot = int(Type::List);
  if (vm.members[kSlot].size() > 0) {
    return;
  }

  const auto members = {
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("clear"), 0, [](Vm& vm, std::size_t) {
        const auto self = vm.stack.top()->as<DzList>();
        self->values.clear();
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("resize"), 2, [](Vm& vm, std::size_t) {
        const auto init = vm.stack.pop_value();
        const auto size = vm.stack.pop_value();
        const auto self = vm.stack.top()->as<DzList>();
        vm.expect(size, DzValue::Type::Int);
        if (size.i < 0) {
          vm.raise("negative resize size");
        }
        self->values.resize(size.i, init);
        return &null;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("push"), 1, [](Vm& vm, std::size_t) -> dzint {
        const auto value = vm.stack.pop_value();
        const auto self  = vm.stack.top()->as<DzList>();
        self->values.push_back(value);
        return self->size();
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("insert"), 2, [](Vm& vm, std::size_t) -> dzint {
        const auto value = vm.stack.pop_value();
        const auto index = vm.stack.pop_value();
        const auto self  = vm.stack.top()->as<DzList>();
        self->values.insert(self->values.begin() + self->toInsertIndex(vm, index), value);
        return self->size();
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("pop"), 0, [](Vm& vm, std::size_t) {
        const auto self = vm.stack.top()->as<DzList>();
        if (self->size() == 0) {
          vm.raise("pop from empty list");
        }
        return self->values.pop_back_value();
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("remove"), 1, [](Vm& vm, std::size_t) {
        const auto self  = vm.stack.peek(1)->as<DzList>();
        const auto index = self->toIndex(vm, vm.stack.pop_value());
        const auto value = self->values[index];
        self->values.erase(self->values.begin() + index);
        return value;
      }
    ),
    vm.gc.constructNoCollect<DzFunction>(
      vm.gc.constructNoCollect<DzString>("index"), 1, [](Vm& vm, std::size_t) -> dzint {
        const auto find = vm.stack.pop_value();
        const auto self = vm.stack.top()->as<DzList>();
        for (const auto [index, value] : sh::enumerate(self->values)) {
          if (value == find) {
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

auto DzList::toIndex(Vm& vm, const DzValue& expr) const -> std::size_t {
  vm.expect(expr, DzValue::Type::Int);
  const auto index = expr.i < 0
    ? expr.i + size()
    : expr.i;
  if (index < 0 || index >= size()) {
    vm.raise("list index out of range");
  }
  return index;
}

auto DzList::toInsertIndex(Vm& vm, const DzValue& expr) const -> std::size_t {
  vm.expect(expr, DzValue::Type::Int);
  const auto index = expr.i < 0
    ? expr.i + size()
    : expr.i;
  if (index < 0 || index > size()) {
    vm.raise("list index out of range");
  }
  return index;
}
