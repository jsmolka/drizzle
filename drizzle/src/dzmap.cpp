#include "dzmap.h"

#include <sh/fmt.h>

#include "dznull.h"
#include "vm.h"

using Pair = std::pair<DzValue, DzValue>;

template<>
struct fmt::formatter<Pair> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  };

  template<typename FormatContext>
  auto format(const Pair& pair, FormatContext& ctx) const {
    const auto& [key, value] = pair;
    return fmt::format_to(ctx.out(), "{}: {}", key, value);
  }
};

DzMap::DzMap()
  : DzObject(Type::Map) {}

DzMap::operator bool() const {
  return values.size() > 0;
}

auto DzMap::operator==(const DzObject& other) const -> bool {
  return other.type == Type::Map && other.as<DzMap>()->values == values;
}

auto DzMap::size() const -> std::size_t {
  return values.size();
}

auto DzMap::repr() const -> std::string {
  return fmt::format("{{{}}}", fmt::join(values, ", "));
}

auto DzMap::get(const DzValue& key) const -> std::optional<DzValue> {
  const auto iter = values.find(key);
  return iter != values.end()
    ? iter->second
    : std::optional<DzValue>();
}

void DzMap::set(const DzValue& key, const DzValue& value) {
  values.insert_or_assign(key, value);
}

auto DzMap::in(Vm& vm, const DzValue& value) -> bool {
  return get(value).has_value();
}

auto DzMap::getExpr(Vm& vm, const DzValue& expr) -> DzValue {
  return get(expr).value_or(&null);
}

void DzMap::setExpr(Vm& vm, const DzValue& expr, const DzValue& value) {
  set(expr, value);
}
