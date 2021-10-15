#include "dzclosure.h"

DzClosure::DzClosure()
    : DzObject(Type::Closure)
{

}

std::string_view DzClosure::typeName() const
{
    return "closure";
}

DzClosure::operator bool() const
{
    return true;
}
