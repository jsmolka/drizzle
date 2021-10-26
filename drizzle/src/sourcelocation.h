#pragma once

#include <cstddef>

class SourceLocation
{
public:
    SourceLocation(std::size_t line);

    std::size_t line;
};
