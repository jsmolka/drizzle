#include "dzobject.h"

#include <shell/macros.h>

#include "dzclosure.h"
#include "dzfunction.h"
#include "dzstring.h"

DzObject::DzObject(DzObject::Type type)
    : type(type)
{

}

std::string_view DzObject::typeName() const
{
    switch (type)
    {
    case Type::String:   return as<DzString>().typeName();
    case Type::Function: return as<DzFunction>().typeName();
    case Type::Closure:  return as<DzClosure>().typeName();

    default:
        SHELL_UNREACHABLE;
        return "unreachable";
    }
}

DzObject::operator bool() const
{
    switch (type)
    {
    case Type::String:   return static_cast<bool>(as<DzString>());
    case Type::Function: return static_cast<bool>(as<DzFunction>());
    case Type::Closure:  return static_cast<bool>(as<DzClosure>());

    default:
        SHELL_UNREACHABLE;
        return false;
    }
}

bool DzObject::operator==(const DzObject& other) const
{
    return this == &other;
}

bool DzObject::operator!=(const DzObject& other) const
{
    return !(this == &other);
}

template<typename T>
const T& DzObject::as() const
{
    static_assert(std::is_base_of_v<DzObject, T>);

    return *static_cast<const T*>(this);
}
