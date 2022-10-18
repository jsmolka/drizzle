#include "dzobject.h"

#include "dzboundmethod.h"
#include "dzbytes.h"
#include "dzclass.h"
#include "dzfunction.h"
#include "dzinstance.h"
#include "dziterator.h"
#include "dzlist.h"
#include "dzmap.h"
#include "dznull.h"
#include "dzrange.h"
#include "dzsdlwindow.h"
#include "dzstring.h"
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

auto DzObject::kind() const -> std::string_view {
  return fmt::formatter<Type>::repr(type);
}

auto DzObject::is(Type type) const -> bool {
  return this->type == type;
}

auto DzObject::subscriptGet(Vm& vm, const DzValue& expr) -> DzValue {
  throw NotSupportedException();
}

void DzObject::subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value) {
  throw NotSupportedException();
}
