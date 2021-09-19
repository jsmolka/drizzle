#include "value.h"

Value::Value()
    : type(Type::Null)
{

}

bool Value::isBitwise() const
{
    return static_cast<int>(type) < static_cast<int>(Type::Float);
}

bool Value::isPrimitive() const
{
    return static_cast<int>(type) < static_cast<int>(Type::Null);
}

std::string_view Value::typeName() const
{
    switch (type)
    {
    case Type::Int:   return "int";
    case Type::Float: return "float";
    case Type::Bool:  return "bool";
    case Type::Null:  return "null";

    default:
        return "unknown";
    }
}

Value::operator bool() const
{
    switch (type)
    {
    case Type::Int:   return static_cast<bool>(i);
    case Type::Float: return static_cast<bool>(f);
    case Type::Bool:  return b;
    case Type::Null:  return false;

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
    case Value::Type::Int:   return i == other.i;
    case Value::Type::Float: return f == other.f;
    case Value::Type::Bool:  return b == other.b;
    case Value::Type::Null:  return true;

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}
bool Value::operator!=(const Value& other) const
{
    return !(*this == other);
}
