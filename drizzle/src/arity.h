#pragma once

#include <string>

class Arity {
public:
  static auto equal(std::size_t value) -> Arity;
  static auto greaterEqual(std::size_t value) -> Arity;

  auto matches(std::size_t value) const -> bool;
  auto message(std::size_t value) const -> std::string;

private:
  enum class Compare { Equal, GreaterEqual };

  Arity(std::size_t value, Compare compare);

  std::size_t value;
  Compare compare;
};
