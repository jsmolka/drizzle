#pragma once

#include <shell/format.h>
#include <shell/macros.h>

#include "expression.h"
#include "statement.h"

template<>
struct fmt::formatter<Expression::Type>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    static constexpr std::string_view repr(const Expression::Type& value)
    {
        static_assert(int(Expression::Type::LastEnumValue) == 4);

        switch (value)
        {
        case Expression::Type::Binary:  return "binary";
        case Expression::Type::Group:   return "group";
        case Expression::Type::Literal: return "literal";
        case Expression::Type::Unary:   return "unary";

        default:
            SHELL_UNREACHABLE;
            return "unreachable";
        }
    }

    template<typename FormatContext>
    auto format(const Expression::Type& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), repr(value));
    }
};

template<>
struct fmt::formatter<Expression::Binary::Type>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    static constexpr std::string_view repr(const Expression::Binary::Type& value)
    {
        static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

        switch (value)
        {
        case Expression::Binary::Type::Addition:        return "+";
        case Expression::Binary::Type::And:             return "&&";
        case Expression::Binary::Type::BitwiseAnd:      return "&";
        case Expression::Binary::Type::BitwiseAsr:      return ">>";
        case Expression::Binary::Type::BitwiseLsl:      return "<<";
        case Expression::Binary::Type::BitwiseLsr:      return ">>>";
        case Expression::Binary::Type::BitwiseOr:       return "||";
        case Expression::Binary::Type::BitwiseXor:      return "^";
        case Expression::Binary::Type::Division:        return "/";
        case Expression::Binary::Type::Equal:           return "==";
        case Expression::Binary::Type::Greater:         return ">";
        case Expression::Binary::Type::GreaterEqual:    return ">=";
        case Expression::Binary::Type::IntegerDivision: return "//";
        case Expression::Binary::Type::Less:            return "<";
        case Expression::Binary::Type::LessEqual:       return "<=";
        case Expression::Binary::Type::Modulo:          return "%";
        case Expression::Binary::Type::Multiplication:  return "*";
        case Expression::Binary::Type::NotEqual:        return "!=";
        case Expression::Binary::Type::Or:              return "||";
        case Expression::Binary::Type::Power:           return "**";
        case Expression::Binary::Type::Subtraction:     return "-";

        default:
            SHELL_UNREACHABLE;
            return "unreachable";
        }
    }

    template<typename FormatContext>
    auto format(const Expression::Binary::Type& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), repr(value));
    }
};

template<>
struct fmt::formatter<Expression::Literal>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Expression::Literal& value, FormatContext& ctx)
    {
        static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);

        switch (value.type)
        {
        case Expression::Literal::Type::Boolean: return fmt::format_to(ctx.out(), "{}", std::get<dzbool>(value.value));
        case Expression::Literal::Type::Float:   return fmt::format_to(ctx.out(), "{}", std::get<dzfloat>(value.value));
        case Expression::Literal::Type::Integer: return fmt::format_to(ctx.out(), "{}", std::get<dzint>(value.value));
        case Expression::Literal::Type::Null:    return fmt::format_to(ctx.out(), "null");
        case Expression::Literal::Type::String:  return fmt::format_to(ctx.out(), std::get<std::string>(value.value));

        default:
            SHELL_UNREACHABLE;
            return fmt::format_to(ctx.out(), "unreachable");
        }
    }
};

template<>
struct fmt::formatter<Expression::Unary::Type>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    static constexpr std::string_view repr(const Expression::Unary::Type& value)
    {
        static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);

        switch (value)
        {
        case Expression::Unary::Type::BitwiseComplement: return "~";
        case Expression::Unary::Type::Minus:             return "-";
        case Expression::Unary::Type::Not:               return "!";

        default:
            SHELL_UNREACHABLE;
            return "unreachable";
        }
    }

    template<typename FormatContext>
    auto format(const Expression::Unary::Type& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), repr(value));
    }
};

template<>
struct fmt::formatter<Statement::Type>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    static constexpr std::string_view repr(const Statement::Type& value)
    {
        static_assert(int(Statement::Type::LastEnumValue) == 3);

        switch (value)
        {
        case Statement::Type::Block:                 return "block";
        case Statement::Type::ExpressionStatement:   return "expression_statement";
        case Statement::Type::Program:               return "program";

        default:
            SHELL_UNREACHABLE;
            return "unreachable";
        }
    }

    template<typename FormatContext>
    auto format(const Statement::Type& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), repr(value));
    }
};
