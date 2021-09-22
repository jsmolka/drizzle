#pragma once

#include <string_view>

class DzObject
{
public:
    enum class Type { String };

    DzObject(Type type);

    std::string_view typeName() const;

    operator bool() const;

    const Type type;

private:
    template<typename T>
    const T& as() const;
};
