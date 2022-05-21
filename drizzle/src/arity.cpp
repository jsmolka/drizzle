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

auto Arity::message() const -> std::string_view {
  static constexpr std::string_view kMessages[2][2] = {
    {
      "expected {} arguments but got {}",
      "expected at least {} arguments but got "
    },
    {
      "expected {} argument but got {}",
      "expected at least {} argument but got {}"
    }
  };
  return kMessages[value == 1][compare == Compare::GreaterEqual];
}

auto Arity::matches(std::size_t value) const -> bool {
  return compare == Compare::Equal
    ? value == this->value
    : value >= this->value;
}
