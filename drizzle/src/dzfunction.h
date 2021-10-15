#pragma once

#include "chunk.h"
#include "dzobject.h"

class DzFunction : public DzObject
{
public:
    DzFunction();

    std::string_view typeName() const;

    operator bool() const;

    int arity = 0;
    Chunk chunk;
    std::string_view identifier;
};
