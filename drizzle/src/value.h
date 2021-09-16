#pragma once

#include <vector>
#include <shell/format.h>
#include <shell/macros.h>

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

    operator bool() const;
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;
};

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
        case Value::Type::Null:     return fmt::format_to(ctx.out(), "null");
        case Value::Type::Boolean:  return fmt::format_to(ctx.out(), "{}", value.boolean);
        case Value::Type::Integer:  return fmt::format_to(ctx.out(), "{}", value.integer);
        case Value::Type::Rational: return fmt::format_to(ctx.out(), "{}", value.rational);

        default:
            SHELL_UNREACHABLE;
            return fmt::format_to(ctx.out(), "unreachable");
        }
    }
};

using Values = std::vector<Value>;
