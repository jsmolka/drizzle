#pragma once

#include <robin_hood/robin_hood.h>

struct Prehashed {
  template <typename T>
  constexpr T&& operator()(T&& value) const {
    return std::forward<T>(value);
  }
};

template <typename T>
using DzMap = robin_hood::unordered_flat_map<std::size_t, T, Prehashed>;
