#include "dzinstance.h"

#include <sh/fmt.h>

DzInstance::DzInstance(DzClass* class_)
  : DzObject(Type::Instance), class_(class_) {}

DzInstance::operator bool() const {
  return true;
}

auto DzInstance::repr() const -> std::string {
  return fmt::format("<{} instance at {}>", name(), fmt::ptr(this));
}

auto DzInstance::name() const -> std::string_view {
  return class_->name();
}
