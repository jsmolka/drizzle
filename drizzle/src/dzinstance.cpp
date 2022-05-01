#include "dzinstance.h"

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzmethod.h"
#include "dzvalue.h"

DzInstance::DzInstance(Gc& gc, DzClass* class_)
  : DzObject(Type::Instance), class_(class_) {
  for (const auto& method : class_->methods) {
    auto bound = gc.construct<DzMethod>(this, method);
    auto string = gc.construct<DzString>(method->identifier);
    fields.insert_or_assign(string, bound);  // Todo: ugly
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
