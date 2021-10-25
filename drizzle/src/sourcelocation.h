#pragma once

#include <string_view>

class SourceLocation
{
public:
    enum class Type { Location, Line };

    SourceLocation(const char* location);
    SourceLocation(std::string_view location);
    SourceLocation(std::size_t line);

    const Type type;
    union
    {
        const char* const location;
        const std::size_t line;
    };
};
