#include "dzinstance.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzInstance::DzInstance(DzClass* class_)
  : DzObject(Type::Instance), class_(class_) {}

DzInstance::operator bool() const {
  return true;
}

auto DzInstance::repr() const -> std::string {
  return fmt::format("<{} instance at 0x{:016X}>", class_->identifier->data, sh::cast<sh::u64>(this));
}

auto DzInstance::get(DzString* identifier) -> std::optional<DzValue> {
  const auto iter = fields.find(identifier);
  return iter != fields.end()
    ? iter->second
    : std::optional<DzValue>();
}

void DzInstance::set(DzString* name, const DzValue& value) {
  fields.insert_or_assign(name, value);
}
