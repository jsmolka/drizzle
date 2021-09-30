#include "dzvalue.h"

#include <shell/macros.h>

DzValue::DzValue()
{
    type = Type::Null;
}

bool DzValue::isString() const
{
    return type == Type::Object && o->type == DzObject::Type::String;
}

std::string_view DzValue::typeName() const
{
    switch (type)
    {
    case Type::Bool:   return "bool";
    case Type::Int:    return "int";
    case Type::Float:  return "float";
    case Type::Null:   return "null";
    case Type::Object: return o->typeName();

    default:
        return "unknown";
    }
}

DzValue::operator bool() const
{
    switch (type)
    {
    case Type::Bool:   return b;
    case Type::Int:    return static_cast<bool>(i);
    case Type::Float:  return static_cast<bool>(f);
    case Type::Null:   return false;
    case Type::Object: return static_cast<bool>(*o);

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}

bool DzValue::operator==(const DzValue& other) const
{
    if (type != other.type)
        return false;

    switch (other.type)
    {
    case DzValue::Type::Bool:   return b == other.b;
    case DzValue::Type::Int:    return i == other.i;
    case DzValue::Type::Float:  return f == other.f;
    case DzValue::Type::Null:   return true;
    case DzValue::Type::Object: return o == other.o;

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}
bool DzValue::operator!=(const DzValue& other) const
{
    return !(*this == other);
}
