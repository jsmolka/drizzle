#include "dzfunction.h"

DzFunction::DzFunction()
    : DzObject(Type::Function)
{

}

std::string_view DzFunction::typeName() const
{
    return "function";
}

DzFunction::operator bool() const
{
    return true;
}
