#include "dzinstance.h"

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzboundmethod.h"
#include "dznull.h"
#include "gc.h"
#include "vm.h"

DzInstance::DzInstance(DzClass* class_)
  : DzObject(Type::Instance), class_(class_) {}

auto DzInstance::repr() const -> std::string {
  return fmt::format("<{} instance at 0x{:016X}>", class_->identifier->data, sh::cast<std::size_t>(this));
}

auto DzInstance::get(const DzString* identifier) const -> std::optional<DzValue> {
  const auto iter = fields.find(identifier);
  return iter != fields.end()
    ? iter->second
    : std::optional<DzValue>();
}

void DzInstance::set(const DzString* name, const DzValue& value) {
  fields.insert_or_assign(name, value);
}

auto DzInstance::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return getProp(vm, expr, true);
}

auto DzInstance::getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue {
  vm.expect(prop, DzObject::Type::String);
  const auto property = prop->as<DzString>();
  if (const auto value = get(property)) {
    return *value;
  } else if (const auto function = class_->get(property)) {
    if (bind) {
      return vm.gc.construct<DzBoundMethod>(this, function);
    } else {
      return function;
    }
  }
  throw NotFoundException();
}

void DzInstance::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  setProp(vm, expr, value);
}

void DzInstance::setProp(Vm& vm, const DzValue& prop, const DzValue& value) {
  vm.expect(prop, DzObject::Type::String);
  set(prop->as<DzString>(), value);
}
