#pragma once

#include <vector>
#include <shell/format.h>
#include <shell/macros.h>

#include "types.h"

struct Value
{
    enum class Type { Int, Float, Bool, Null };

    Value();
    template<typename T>
    Value(const T& value) { set(value); }

    template<typename T>
    void set(const T& value);

    bool isPrimitive() const;
    const char* typeName() const;

    operator bool() const;
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;

    Type type;
    union
    {
        dzint i;
        dzfloat f;
        dzbool b;
    };
};

template<typename T>
void Value::set(const T& value)
{
    static_assert(is_dz_primitive_v<T>);

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
    if constexpr (std::is_same_v<T, dzbool>)
    {
        b = value;
        type = Type::Bool;
    }
}

template<>
struct fmt::formatter<Value>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Value& value, FormatContext& ctx)
    {
        switch (value.type)
        {
        case Value::Type::Int:   return fmt::format_to(ctx.out(), "{}", value.i);
        case Value::Type::Float: return fmt::format_to(ctx.out(), "{}", value.f);
        case Value::Type::Bool:  return fmt::format_to(ctx.out(), "{}", value.b);
        case Value::Type::Null:  return fmt::format_to(ctx.out(), "null");

        default:
            SHELL_UNREACHABLE;
            return fmt::format_to(ctx.out(), "unreachable");
        }
    }
};

using Values = std::vector<Value>;
