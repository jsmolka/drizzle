#pragma once

#include <shell/vector.h>

#include "dzfunction.h"
#include "dzupvalue.h"

class DzClosure : public DzObject
{
public:
    DzClosure(DzFunction& function);

    constexpr std::string_view typeName() const;
    constexpr operator bool() const;

    DzFunction& function;
    shell::Vector<DzUpvalue*, 4> upvalues;
};

constexpr std::string_view DzClosure::typeName() const
{
    return function.typeName();
}

constexpr DzClosure::operator bool() const
{
    return static_cast<bool>(function);
}
