#pragma once

#include <robin_hood/robin_hood.h>

namespace detail {

struct Identity {
  template <typename T>
  constexpr T&& operator()(T&& value) const {
    return std::forward<T>(value);
  }
};

}  // namespace detail

template <typename T>
using DzMap = robin_hood::unordered_flat_map<std::size_t, T, detail::Identity>;
