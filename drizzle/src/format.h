#pragma once

#include <shell/format.h>
#include <shell/macros.h>

#include "dzfunction.h"
#include "dzobject.h"
#include "dzstring.h"
#include "dzvalue.h"

namespace
{

inline bool isWhole(double d)
{
    return std::fmod(d, 1.0) == 0.0;
}

}  // namespace

template<>
struct fmt::formatter<DzValue>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const DzValue& value, FormatContext& ctx)
    {
        switch (value.type)
        {
        case DzValue::Type::Bool:   return fmt::format_to(ctx.out(), "{}", value.b);
        case DzValue::Type::Int:    return fmt::format_to(ctx.out(), "{}", value.i);
        case DzValue::Type::Float:  return fmt::format_to(ctx.out(), isWhole(value.f) ? "{:.1f}" : "{}", value.f);
        case DzValue::Type::Object: return fmt::format_to(ctx.out(), "{}", *value.o);
        case DzValue::Type::Null:   return fmt::format_to(ctx.out(), "null");

        default:
            SHELL_UNREACHABLE;
            return fmt::format_to(ctx.out(), "unreachable");
        }
    }
};

template<>
struct fmt::formatter<DzObject>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const DzObject& object, FormatContext& ctx)
    {
        switch (object.type)
        {
        case DzObject::Type::Function:
            return fmt::format_to(ctx.out(), "<fn {}>", static_cast<const DzFunction*>(&object)->identifier);

        case DzObject::Type::String:
            return fmt::format_to(ctx.out(), "{}", static_cast<const DzString*>(&object)->data);

        default:
            SHELL_UNREACHABLE;
            return fmt::format_to(ctx.out(), "unreachable");
        }
    }
};

