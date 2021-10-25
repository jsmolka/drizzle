#pragma once

#include <string_view>

class SourceLocation
{
public:
    enum class Type { Position, Line };

    SourceLocation(const char* position);
    SourceLocation(std::string_view position);
    SourceLocation(std::size_t line);

    const Type type;
    union
    {
        const char* const position;
        const std::size_t line;
    };
};
