#pragma once

#include "dzobject.h"

class DzFunction : public DzObject
{
public:
    DzFunction();

    std::string_view typeName() const;

    operator bool() const;
};
