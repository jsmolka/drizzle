#pragma once

#include <sh/concepts.h>
#include <sh/traits.h>

#include "int.h"

using dzbool = bool;
template <typename... Ts>
concept dz_bool = std::conjunction_v<std::is_same<dzbool, Ts>...>;

using dzint = s64;
template <typename... Ts>
concept dz_int = std::conjunction_v<std::is_same<dzint, Ts>...>;

using dzfloat = double;
template <typename... Ts>
concept dz_float = std::conjunction_v<std::is_same<dzfloat, Ts>...>;

template <typename... Ts>
concept dz_primitive = std::conjunction_v<sh::is_any_of<Ts, dzbool, dzint, dzfloat>...>;

template <dz_primitive T, dz_primitive U = T>
struct promote {
  using type = std::conditional_t<sh::any_of<dzfloat, T, U>, dzfloat, dzint>;
};

template <dz_primitive T, dz_primitive U = T>
using promote_t = typename promote<T, U>::type;

template <dz_primitive T, dz_primitive U = T>
struct promote_lax {
  using type = std::conditional_t<dz_bool<T, U>, dzbool, promote_t<T, U>>;
};

template <dz_primitive T, dz_primitive U = T>
using promote_lax_t = typename promote_lax<T, U>::type;
