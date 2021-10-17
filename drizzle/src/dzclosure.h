#pragma once

#include "dzfunction.h"

class DzClosure : public DzObject
{
public:
    DzClosure(DzFunction& function);

    std::string_view typeName() const;

    operator bool() const;

    DzFunction& function;
};
