#pragma once

#include "chunk.h"
#include "dzobject.h"

class DzFunction : public DzObject
{
public:
    DzFunction();

    std::string_view typeName() const;

    operator bool() const;

    Chunk chunk;
    std::size_t arity = 0;
    std::size_t upvalues = 0;
    std::string_view identifier;
};
