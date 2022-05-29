#include "dzobject.h"

#include "dzboundmethod.h"
#include "dzclass.h"
#include "dzfunction.h"
#include "dzinstance.h"
#include "dziterator.h"
#include "dzlist.h"
#include "dznull.h"
#include "dzrange.h"
#include "dzreverseiterator.h"
#include "dzstring.h"

DzObject::DzObject(Type type)
  : type(type) {}

DzObject::operator bool() const {
  switch (type) {
    case Type::BoundMethod:     return *as<DzBoundMethod>();
    case Type::Class:           return *as<DzClass>();
    case Type::Function:        return *as<DzFunction>();
    case Type::Instance:        return *as<DzInstance>();
    case Type::Iterator:        return *as<DzIterator>();
    case Type::List:            return *as<DzList>();
    case Type::Null:            return *as<DzNull>();
    case Type::Range:           return *as<DzRange>();
    case Type::ReverseIterator: return *as<DzReverseIterator>();
    case Type::String:          return *as<DzString>();
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
    case Type::List:        return *as<DzList>() == *other.as<DzList>();
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
    case Type::BoundMethod:     return as<DzBoundMethod>()->repr();
    case Type::Class:           return as<DzClass>()->repr();
    case Type::Function:        return as<DzFunction>()->repr();
    case Type::Instance:        return as<DzInstance>()->repr();
    case Type::Iterator:        return as<DzIterator>()->repr();
    case Type::List:            return as<DzList>()->repr();
    case Type::Null:            return as<DzNull>()->repr();
    case Type::Range:           return as<DzRange>()->repr();
    case Type::ReverseIterator: return as<DzReverseIterator>()->repr();
    case Type::String:          return as<DzString>()->repr();
    default:
      SH_UNREACHABLE;
      return "unreachable";
  }
}

auto DzObject::is(Type type) const -> bool {
    return this->type == type;
}
