#pragma once

#include <vector>

#include "int.h"

struct Value
{
    Value();
    Value(bool boolean);
    Value(s64 integer);
    Value(double rational);

    enum class Type
    {
        Null,
        Boolean,
        Integer,
        Rational
    } type;

    union
    {
        bool boolean;
        s64 integer;
        double rational;
    };

    void print();
};

using Values = std::vector<Value>;
