#pragma once

#include <string_view>

class DzObject
{
public:
    enum class Type { String };

    DzObject(Type type);

    std::string_view typeName() const;

    operator bool() const;
    bool operator==(const DzObject& other);
    bool operator!=(const DzObject& other);

    const Type type;

private:
    template<typename T>
    const T& as() const;
};
