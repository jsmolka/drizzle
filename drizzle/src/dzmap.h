#pragma once

#include <tsl/robin_map.h>

namespace detail {

struct Identity {
  template <typename T>
  constexpr T&& operator()(T&& value) const {
    return std::forward<T>(value);
  }
};

}  // namespace detail

template <typename T>
using DzMap = tsl::robin_map<std::size_t, T, detail::Identity>;
