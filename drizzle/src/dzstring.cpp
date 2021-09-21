#include "dzstring.h"

DzString::DzString(const std::string& data)
    : DzObject(Type::String)
    , data(data)
    , hash(robin_hood::hash<std::string>()(data))
{

}

DzString::DzString(std::string&& data)
    : DzObject(Type::String)
    , data(data)
    , hash(robin_hood::hash<std::string>()(data))
{

}

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
