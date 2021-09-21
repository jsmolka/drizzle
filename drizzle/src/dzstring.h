#pragma once

#include <string>
#include <robin_hood.h>

#include "dzobject.h"

class DzString : public DzObject
{
public:
    DzString(const std::string& data);
    DzString(std::string&& data);

    std::string_view typeName() const;

    operator bool() const;
    bool operator==(const DzString& other);
    bool operator!=(const DzString& other);

    const std::string data;
    const std::size_t hash;
};

namespace robin_hood
{

template<>
struct hash<DzString>
{
    std::size_t operator()(const DzString& string) const noexcept
    {
        return string.hash;
    }
};

}  // namespace robin_hood
