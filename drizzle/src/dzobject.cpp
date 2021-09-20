#include "dzobject.h"

#include <shell/macros.h>

#include "dzstring.h"

std::string_view DzObject::typeName() const
{
    switch (type)
    {
    case Type::String:
        return as<DzString>().typeName();

    default:
        SHELL_UNREACHABLE;
        return "unreachable";
    }
}

DzObject::operator bool() const
{
    switch (type)
    {
    case Type::String:
        return static_cast<bool>(as<DzString>());

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}

bool DzObject::operator==(const DzObject& other)
{
    if (type != other.type)
        return false;

    switch (type)
    {
    case Type::String:
        return as<DzString>() == other.as<DzString>();

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}

bool DzObject::operator!=(const DzObject& other)
{
    return !(*this == other);
}

template<typename T>
const T& DzObject::as() const
{
    static_assert(std::is_base_of_v<DzObject, T>);

    return *static_cast<const T*>(this);
}
