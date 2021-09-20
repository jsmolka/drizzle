#pragma once

#include "dzobject.h"

class DzString : public DzObject
{
public:
    std::string_view typeName() const;

    operator bool() const;
    bool operator==(const DzString& other);
    bool operator!=(const DzString& other);

    std::string data;
};
