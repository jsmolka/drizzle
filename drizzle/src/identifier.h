#pragma once

#include <string_view>

#include <sh/fmt.h>

#include "location.h"

class Identifier : public std::string_view {
public:
  template<typename T>
    requires std::constructible_from<T, std::string_view>
  Identifier(const T& value, const Location& location)
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
