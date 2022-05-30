#include "dzmap.h"

#include <sh/fmt.h>

using Pair = std::pair<DzString*, DzValue>;

template<>
struct fmt::formatter<Pair> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  };

  template<typename FormatContext>
  auto format(const Pair& pair, FormatContext& ctx) const {
    const auto& [key, value] = pair;
    return fmt::format_to(ctx.out(), "{}: {}", key->repr(), value);
  }
};

DzMap::DzMap()
  : DzObject(Type::Map) {}

DzMap::operator bool() const {
  return values.size() > 0;
}

auto DzMap::operator==(const DzMap& other) const -> bool {
  return values == other.values;
}

auto DzMap::repr() const -> std::string {
  return fmt::format("{{{}}}", fmt::join(values, ", "));
}

auto DzMap::get(DzString* identifier) -> std::optional<DzValue> {
  const auto iter = values.find(identifier);
  return iter != values.end()
    ? iter->second
    : std::optional<DzValue>();
}

void DzMap::set(DzString* identifier, const DzValue& value) {
  values.insert_or_assign(identifier, value);
}
