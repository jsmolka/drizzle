#include "dzobject.h"

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzboundmethod.h"
#include "gc.h"
#include "vm.h"

DzObject::DzObject(Type type)
  : type(type), marked(false), next(nullptr) {}

DzObject::operator bool() const {
  return true;
}

auto DzObject::operator==(const DzObject& other) const -> bool {
  return this == &other;
}

auto DzObject::hash() const -> std::size_t {
  return std::hash<const void*>{}(this);
}

auto DzObject::size() const -> std::size_t {
  throw NotSupportedException();
}

auto DzObject::repr() const -> std::string {
  return fmt::format("<{} at 0x{:016X}>", type, sh::cast<std::size_t>(this));
}

auto DzObject::kind() const -> std::string_view {
  return fmt::formatter<Type>::repr(type);
}

auto DzObject::is(Type type) const -> bool {
  return this->type == type;
}

auto DzObject::makeIterator(Vm& vm) -> DzValue {
  throw NotSupportedException();
}

auto DzObject::makeReverseIterator(Vm& vm) -> DzValue {
  throw NotSupportedException();
}

auto DzObject::getItem(Vm& vm, std::size_t) -> DzValue {
  throw NotSupportedException();
}

auto DzObject::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  throw NotSupportedException();
}

auto DzObject::getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue {
  vm.expect(prop, DzObject::Type::String);
  const auto type = int(this->type);
  const auto iter = vm.members[type].find(prop->as<DzString>());
  if (iter != vm.members[type].end()) {
    const auto& [identifier, function] = *iter;
    if (bind) {
      return vm.gc.construct<DzBoundMethod>(this, function);
    } else {
      return function;
    }
  } else {
    throw NotFoundException();
  }
}

void DzObject::setItem(Vm& vm, std::size_t index, const DzValue& value) {
  throw NotSupportedException();
}

void DzObject::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  throw NotSupportedException();
}

void DzObject::setProp(Vm& vm, const DzValue& prop, const DzValue& value) {
  throw NotSupportedException();
}
