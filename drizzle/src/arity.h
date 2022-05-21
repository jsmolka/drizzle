#pragma once

#include <string_view>

class Arity {
public:
  static auto equal(std::size_t value) -> Arity;
  static auto greaterEqual(std::size_t value) -> Arity;

  auto message() const -> std::string_view;
  auto matches(std::size_t value) const -> bool;

  std::size_t value;

private:
  enum class Compare { Equal, GreaterEqual };

  Arity(std::size_t value, Compare compare);

  Compare compare;
};
