#pragma once

#include "dzobject.h"

class DzValue;

class DzUpvalue : public DzObject
{
public:
    DzUpvalue(DzValue& value);

    constexpr std::string_view typeName() const;
    constexpr operator bool() const;

    DzValue& value;
};

constexpr std::string_view DzUpvalue::typeName() const
{
    return "upvalue";
}

constexpr DzUpvalue::operator bool() const
{
    return true;
}
