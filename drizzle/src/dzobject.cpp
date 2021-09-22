#include "dzobject.h"

#include <shell/macros.h>

#include "dzstring.h"

DzObject::DzObject(DzObject::Type type)
    : type(type)
{

}

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

template<typename T>
const T& DzObject::as() const
{
    static_assert(std::is_base_of_v<DzObject, T>);

    return *static_cast<const T*>(this);
}
