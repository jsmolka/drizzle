#pragma once

#include "dzfunction.h"

class DzClosure : public DzObject
{
public:
    DzClosure();

    std::string_view typeName() const;

    operator bool() const;

    DzFunction* function;
};
