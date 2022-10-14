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
  : type(type) {}

DzObject::operator bool() const {
  switch (type) {
    case Type::BoundMethod: return *as<DzBoundMethod>();
    case Type::Bytes:       return *as<DzBytes>();
    case Type::Class:       return *as<DzClass>();
    case Type::Function:    return *as<DzFunction>();
    case Type::Instance:    return *as<DzInstance>();
    case Type::Iterator:    return *as<DzIterator>();
    case Type::List:        return *as<DzList>();
    case Type::Map:         return *as<DzMap>();
    case Type::Null:        return *as<DzNull>();
    case Type::Range:       return *as<DzRange>();
    case Type::SdlWindow:   return *as<DzSdlWindow>();
    case Type::String:      return *as<DzString>();
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzObject::operator==(const DzObject& other) const -> bool {
  if (type != other.type) {
    return false;
  }
  switch (type) {
    case Type::BoundMethod: return *as<DzBoundMethod>() == *other.as<DzBoundMethod>();
    case Type::Bytes:       return *as<DzBytes>() == *other.as<DzBytes>();
    case Type::List:        return *as<DzList>() == *other.as<DzList>();
    case Type::Map:         return *as<DzMap>() == *other.as<DzMap>();
    case Type::Range:       return *as<DzRange>() == *other.as<DzRange>();
    default:
      return this == &other;
  }
}

auto DzObject::operator!=(const DzObject& other) const -> bool {
  return !(*this == other);
}

auto DzObject::repr() const -> std::string {
  switch (type) {
    case Type::BoundMethod: return as<DzBoundMethod>()->repr();
    case Type::Bytes:       return as<DzBytes>()->repr();
    case Type::Class:       return as<DzClass>()->repr();
    case Type::Function:    return as<DzFunction>()->repr();
    case Type::Instance:    return as<DzInstance>()->repr();
    case Type::Iterator:    return as<DzIterator>()->repr();
    case Type::List:        return as<DzList>()->repr();
    case Type::Map:         return as<DzMap>()->repr();
    case Type::Null:        return as<DzNull>()->repr();
    case Type::Range:       return as<DzRange>()->repr();
    case Type::SdlWindow:   return as<DzSdlWindow>()->repr();
    case Type::String:      return as<DzString>()->repr();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
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
