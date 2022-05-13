#pragma once

#include <string>
#include <string_view>

#include <sh/fmt.h>

#include "location.h"

class Identifier : public std::string_view {
public:
  Identifier(const char* string, const Location& location = Location())
    : std::string_view(string), location(location) {}
  Identifier(std::string_view string, const Location& location = Location())
    : std::string_view(string), location(location) {}
  Identifier(const std::string& string, const Location& location = Location())
    : std::string_view(string), location(location) {}

  Location location;
};

template<>
struct fmt::formatter<Identifier> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Identifier& identifier, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(identifier, ctx);
  }
};

namespace std {

template<>
struct hash<Identifier> {
  auto operator()(const Identifier& identifier) const -> std::size_t {
    return hash<std::string_view>()(identifier);
  }
};

}  // namespace std
