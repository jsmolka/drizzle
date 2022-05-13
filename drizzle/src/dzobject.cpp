#include "dzobject.h"

#include <sh/utility.h>

#include "dzboundmethod.h"
#include "dzclass.h"
#include "dzfunction.h"
#include "dzinstance.h"
#include "dznull.h"
#include "dzstring.h"

DzObject::DzObject(Type type)
  : type(type) {}

DzObject::operator bool() const {
  switch (type) {
    case Type::BoundMethod: return as<DzBoundMethod>();
    case Type::Class:       return as<DzClass>();
    case Type::Function:    return as<DzFunction>();
    case Type::Instance:    return as<DzInstance>();
    case Type::Null:        return as<DzNull>();
    case Type::String:      return as<DzString>();
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzObject::operator==(const DzObject& other) const -> bool {
  return this == &other;
}

auto DzObject::operator!=(const DzObject& other) const -> bool {
  return !(*this == other);
}

auto DzObject::kind() const -> std::string_view {
  switch (type) {
    case Type::BoundMethod: return as<DzBoundMethod>().kind();
    case Type::Class:       return as<DzClass>().kind();
    case Type::Function:    return as<DzFunction>().kind();
    case Type::Instance:    return as<DzInstance>().kind();
    case Type::Null:        return as<DzNull>().kind();
    case Type::String:      return as<DzString>().kind();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

auto DzObject::repr() const -> std::string {
  switch (type) {
    case Type::BoundMethod: return as<DzBoundMethod>().repr();
    case Type::Class:       return as<DzClass>().repr();
    case Type::Function:    return as<DzFunction>().repr();
    case Type::Instance:    return as<DzInstance>().repr();
    case Type::Null:        return as<DzNull>().repr();
    case Type::String:      return as<DzString>().repr();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

auto DzObject::is(Type type) const -> bool {
    return this->type == type;
}

template<typename T>
auto DzObject::as() const -> const T& {
  return *static_cast<const T*>(this);
}
