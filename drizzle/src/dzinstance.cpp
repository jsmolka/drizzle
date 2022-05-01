#include "dzinstance.h"

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzmethod.h"
#include "dznull.h"
#include "dzvalue.h"

DzInstance::DzInstance(Gc& gc, DzClass* class_)
  : DzObject(Type::Instance), class_(class_) {
  for (const auto& function : class_->methods) {
    auto string = gc.construct<DzString>(function->identifier);
    auto method = gc.construct<DzMethod>(this, function);
    fields.insert({ string, method });
  }
}

DzInstance::operator bool() const {
  return true;
}

auto DzInstance::repr() const -> std::string {
  return fmt::format("<{} object at 0x{:016X}>", name(), sh::cast<sh::u64>(this));
}

auto DzInstance::name() const -> std::string_view {
  return class_->name();
}

auto DzInstance::get(DzString* name) -> DzValue {
  auto pos = fields.find(name);
  return pos != fields.end()
    ? pos->second
    : &null;
}

void DzInstance::set(DzString* name, const DzValue& value) {
  fields.insert_or_assign(name, value);
}
