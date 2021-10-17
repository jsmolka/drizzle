#include "dzclosure.h"

DzClosure::DzClosure(DzFunction& function)
    : DzObject(Type::Closure), function(function)
{

}

std::string_view DzClosure::typeName() const
{
    return function.typeName();
}

DzClosure::operator bool() const
{
    return static_cast<bool>(function);
}
