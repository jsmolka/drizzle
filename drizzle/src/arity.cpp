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
  static constexpr const char* kFormats[] = {
    "expected {} argument{} but got {}",
    "expected at least {} argument{} but got {}"
  };
  const auto format = kFormats[int(compare)];
  const auto ending = this->value == 1 ? "" : "s";
  return fmt::format(fmt::runtime(format), this->value, ending, value);
}
