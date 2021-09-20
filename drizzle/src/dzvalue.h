#pragma once

#include <vector>

#include "dzobject.h"
#include "dztypes.h"

struct DzValue
{
    enum class Type { Bool, Int, Float, Null, Object };

    DzValue();
    DzValue(dzbool    value);
    DzValue(dzint     value);
    DzValue(dzfloat   value);
    DzValue(DzObject* value);

    void set(dzbool    value);
    void set(dzint     value);
    void set(dzfloat   value);
    void set(DzObject* value);

    bool isBitwise() const;
    bool isPrimitive() const;
    std::string_view typeName() const;

    operator bool() const;
    bool operator==(const DzValue& other) const;
    bool operator!=(const DzValue& other) const;

    Type type;
    union
    {
        dzint i;
        dzfloat f;
        dzbool b;
        DzObject* o;
    };
};

using DzValues = std::vector<DzValue>;