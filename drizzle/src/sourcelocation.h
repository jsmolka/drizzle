#pragma once

#include <cstddef>

class SourceLocation {
public:
  SourceLocation(std::size_t line);

  const std::size_t line;
  const std::size_t column;
};
