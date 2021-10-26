#pragma once

#include <string>

#include "dzobject.h"

class DzString : public DzObject
{
public:
    DzString(std::string&& data, std::size_t hash);

    constexpr std::string_view typeName() const;

    operator bool() const;

    const std::string data;
    const std::size_t hash;
};

constexpr std::string_view DzString::typeName() const
{
    return "string";
}
