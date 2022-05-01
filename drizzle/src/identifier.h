#pragma once

#include <string_view>

#include <sh/fmt.h>

#include "location.h"

class Identifier : public std::string_view {
public:
  Identifier(const char* value, const Location& location = Location())
    : std::string_view(value), location(location) {}
  Identifier(std::string_view value, const Location& location = Location())
    : std::string_view(value), location(location) {}

  Location location;
};

template<>
struct fmt::formatter<Identifier> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Identifier& identifier, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(identifier, ctx);
  }
};
