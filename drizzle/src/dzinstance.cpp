#include "dzinstance.h"

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

void DzInstance::mark(Gc& gc) {
  DzObject::mark(gc);
  gc.mark(class_);
  for (const auto& [key, value] : fields) {
    gc.mark(key);
    gc.mark(value);
  }
}

auto DzInstance::in(Vm& vm, const DzValue& value) -> bool {
  vm.expect(value, DzObject::Type::String);
  return fields.contains(value->as<DzString>()) || class_->functions.contains(value->as<DzString>());
}

auto DzInstance::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return getProp(vm, expr, true);
}

auto DzInstance::getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue {
  vm.expect(prop, DzObject::Type::String);
  if (const auto value = fields.get(prop->as<DzString>())) {
    return *value;
  } else if (const auto function = class_->get(prop->as<DzString>())) {
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
  fields.set(prop->as<DzString>(), value);
}
