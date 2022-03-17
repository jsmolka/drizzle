#include "dzinstance.h"

#include <sh/fmt.h>
#include <sh/utility.h>

DzInstance::DzInstance(DzClass* class_)
  : DzObject(Type::Instance), class_(class_) {}

DzInstance::operator bool() const {
  return true;
}

auto DzInstance::repr() const -> std::string {
  return fmt::format("<{} object at 0x{:016X}>", name(), sh::cast<sh::u64>(this));
}

auto DzInstance::name() const -> std::string_view {
  return class_->name();
}
