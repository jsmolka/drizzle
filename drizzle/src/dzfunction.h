#pragma once

#include "chunk.h"
#include "dzobject.h"

class DzFunction : public DzObject
{
public:
    DzFunction();

    constexpr std::string_view typeName() const;
    constexpr operator bool() const;

    Chunk chunk;
    std::size_t arity = 0;
    std::size_t upvalues = 0;
    std::string_view identifier;
};

constexpr std::string_view DzFunction::typeName() const
{
    return "function";
}

constexpr DzFunction::operator bool() const
{
    return true;
}
