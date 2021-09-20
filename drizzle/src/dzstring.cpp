#include "dzstring.h"

std::string_view DzString::typeName() const
{
    return "string";
}

DzString::operator bool() const
{
    return data.size() > 0;
}

bool DzString::operator==(const DzString& other)
{
    return data == other.data;
}

bool DzString::operator!=(const DzString& other)
{
    return data != other.data;
}
