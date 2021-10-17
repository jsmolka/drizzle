#pragma once

#include <vector>

#include "dzobject.h"
#include "dzprimitives.h"

struct DzNull {};

template<typename... Ts>
inline constexpr auto is_dz_null_v = shell::is_same_v<DzNull, Ts...>;

template<typename... Ts>
struct is_dz_null : std::bool_constant<is_dz_null_v<Ts...>> {};

class DzValue
{
public:
    enum class Type { Bool, Int, Float, Null, Object, LastEnumValue };

    DzValue();
    template<typename T>
    DzValue(const T& value);
    template<typename T>
    DzValue& operator=(const T& value);

    std::string_view typeName() const;

    operator bool() const;

    Type type;
    union
    {
        dzbool b;
        dzint i;
        dzfloat f;
        DzObject* o;
    };
};

using DzValues = std::vector<DzValue>;

template<typename T>
DzValue::DzValue(const T& value)
{
    *this = value;
}

template<typename T>
DzValue& DzValue::operator=(const T& value)
{
    static_assert(is_dz_primitive_v<T> || is_dz_object_v<T>);

    if constexpr (is_dz_bool_v<T>)   { b = value; type = Type::Bool;   }
    if constexpr (is_dz_int_v<T>)    { i = value; type = Type::Int;    }
    if constexpr (is_dz_float_v<T>)  { f = value; type = Type::Float;  }
    if constexpr (is_dz_object_v<T>) { o = value; type = Type::Object; }
    return *this;
}
