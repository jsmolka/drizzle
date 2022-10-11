#include "dzinstance.h"

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzboundmethod.h"
#include "dznull.h"
#include "gc.h"
#include "vm.h"

DzInstance::DzInstance(DzClass* class_)
  : DzObject(Type::Instance), class_(class_) {}

DzInstance::operator bool() const {
  return true;
}

auto DzInstance::repr() const -> std::string {
  return fmt::format("<{} instance at 0x{:016X}>", class_->identifier->data, sh::cast<sh::u64>(this));
}

auto DzInstance::get(DzString* identifier) const -> std::optional<DzValue> {
  const auto iter = fields.find(identifier);
  return iter != fields.end()
    ? iter->second
    : std::optional<DzValue>();
}

void DzInstance::set(DzString* name, const DzValue& value) {
  fields.insert_or_assign(name, value);
}

auto DzInstance::subscriptGet(Vm& vm, const DzValue& expr) const -> std::optional<DzValue> {
  vm.expect(expr, DzObject::Type::String);
  const auto prop = expr.o->as<DzString>();
  if (const auto value = get(prop)) {
    return *value;
  } else if (const auto function = class_->get(prop)) {
    // Todo: ugly
    return vm.gc.construct<DzBoundMethod>(const_cast<DzInstance*>(this), function);
  } else {
    return &null;
  }
}
