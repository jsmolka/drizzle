#include "value.h"

Value::Value()
    : type(Type::Null) {}

Value::Value(bool boolean)
    : type(Type::Boolean), boolean(boolean) {}

Value::Value(s64 integer)
    : type(Type::Integer), integer(integer) {}

Value::Value(double rational)
    : type(Type::Rational), rational(rational) {}

Value::operator bool() const
{
    switch (type)
    {
    case Type::Null:     return false;
    case Type::Boolean:  return boolean;
    case Type::Integer:  return integer != 0;
    case Type::Rational: return rational != 0;

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}

bool Value::operator==(const Value& other) const
{
    if (type != other.type)
        return false;

    switch (other.type)
    {
    case Value::Type::Null:     return true;
    case Value::Type::Boolean:  return boolean == other.boolean;
    case Value::Type::Integer:  return integer == other.integer;
    case Value::Type::Rational: return rational == other.rational;

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}
bool Value::operator!=(const Value& other) const
{
    return !(*this == other);
}
