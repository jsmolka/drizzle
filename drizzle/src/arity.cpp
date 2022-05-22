#include "arity.h"

#include <sh/fmt.h>

Arity::Arity(std::size_t value, Compare compare)
  : value(value), compare(compare) {}

auto Arity::equal(std::size_t value) -> Arity {
  return {value, Compare::Equal};
}

auto Arity::greaterEqual(std::size_t value) -> Arity {
  return {value, Compare::GreaterEqual};
}

auto Arity::matches(std::size_t value) const -> bool {
  return compare == Compare::Equal
    ? value == this->value
    : value >= this->value;
}

auto Arity::message(std::size_t value) const -> std::string {
  static constexpr std::string_view kFormats[2][2] = {
    {
      "expected {} arguments but got {}",
      "expected at least {} arguments but got {}"
    },
    {
      "expected {} argument but got {}",
      "expected at least {} argument but got {}"
    }
  };
  const auto format = kFormats[this->value == 1][int(compare)];
  return fmt::format(fmt::runtime(format), this->value, value);
}
