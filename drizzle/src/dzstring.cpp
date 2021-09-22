#include "dzstring.h"

DzString::DzString(std::string&& data, std::size_t hash)
    : DzObject(Type::String)
    , data(std::move(data))
    , hash(hash)
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
