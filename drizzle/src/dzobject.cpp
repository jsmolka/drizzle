#include "dzobject.h"

#include <sh/fmt.h>
#include <sh/utility.h>

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
  return fmt::format("<{} at 0x{:016X}>", type, sh::cast<u64>(this));
}

auto DzObject::kind() const -> std::string_view {
  return fmt::formatter<Type>::repr(type);
}

auto DzObject::is(Type type) const -> bool {
  return this->type == type;
}

auto DzObject::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  throw NotSupportedException();
}

void DzObject::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  throw NotSupportedException();
}
