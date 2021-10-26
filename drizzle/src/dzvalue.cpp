#include "dzvalue.h"

#include <shell/macros.h>

DzValue::DzValue()
{
    type = Type::Null;
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
        SHELL_UNREACHABLE;
        return "unreachable";
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
