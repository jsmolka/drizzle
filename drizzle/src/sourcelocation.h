#pragma once

#include <cstddef>
#include <optional>

struct SourceLocation {
  std::size_t line = 0;
  std::optional<std::size_t> column = std::nullopt;
};
