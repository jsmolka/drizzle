#include "dzobject.h"

#include "dzboundmethod.h"
#include "dzclass.h"
#include "dzfunction.h"
#include "dzinstance.h"
#include "dzlist.h"
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
    case Type::List:        return as<DzList>();
    case Type::Null:        return as<DzNull>();
    case Type::String:      return as<DzString>();
    default:
      SH_UNREACHABLE;
      return false;
  }
}

auto DzObject::operator==(const DzObject& other) const -> bool {
  if (type != other.type) {
    return false;
  } else if (type == Type::List) {
    return as<DzList>() == other.as<DzList>();
  } else {
    return this == &other;
  }
}

auto DzObject::operator!=(const DzObject& other) const -> bool {
  return !(*this == other);
}

auto DzObject::repr() const -> std::string {
  switch (type) {
    case Type::BoundMethod: return as<DzBoundMethod>().repr();
    case Type::Class:       return as<DzClass>().repr();
    case Type::Function:    return as<DzFunction>().repr();
    case Type::Instance:    return as<DzInstance>().repr();
    case Type::List:        return as<DzList>().repr();
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
