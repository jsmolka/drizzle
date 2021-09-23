#pragma once

#include <vector>
#include <shell/traits.h>

#include "dzobject.h"
#include "dztypes.h"

namespace
{

template<typename T>
inline constexpr auto is_dz_object_v = std::is_pointer_v<T> && std::is_base_of_v<DzObject, shell::unqualified_t<T>>;

template<typename T>
inline constexpr auto is_dz_value_v = is_dz_primitive_v<T> || is_dz_object_v<T>;

}  // namespace

struct DzValue
{
    enum class Type { Bool, Int, Float, Null, Object };

    DzValue();
    template<typename T, std::enable_if_t<is_dz_value_v<T>, int> = 0>
    DzValue(const T& value);

    bool isString() const;
    bool isBitwise() const;
    bool isPrimitive() const;
    std::string_view typeName() const;

    operator bool() const;
    bool operator==(const DzValue& other) const;
    bool operator!=(const DzValue& other) const;

    template<typename T, std::enable_if_t<is_dz_value_v<T>, int> = 0>
    DzValue& operator=(const T& value);

    Type type;
    union
    {
        dzbool b;
        dzint i;
        dzfloat f;
        DzObject* o;
    };
};

template<typename T, std::enable_if_t<is_dz_value_v<T>, int>>
DzValue::DzValue(const T& value)
{
    *this = value;
}

template<typename T, std::enable_if_t<is_dz_value_v<T>, int>>
DzValue& DzValue::operator=(const T& value)
{
    if constexpr (std::is_same_v<T, dzbool>)
    {
        b = value;
        type = Type::Bool;
    }
    if constexpr (std::is_same_v<T, dzint>)
    {
        i = value;
        type = Type::Int;
    }
    if constexpr (std::is_same_v<T, dzfloat>)
    {
        f = value;
        type = Type::Float;
    }
    if constexpr (is_dz_object_v<T>)
    {
        o = value;
        type = Type::Object;
    }
    return *this;
}

using DzValues = std::vector<DzValue>;
