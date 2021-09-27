#pragma once

#include <shell/traits.h>

#include "int.h"

using dzint   = s64;
using dzfloat = double;
using dzbool  = bool;

template<typename T>
inline constexpr auto is_dz_primitive_v = shell::is_any_of_v<T, dzint, dzfloat, dzbool>;

template<typename T, typename U>
struct promoted
{
    static_assert(is_dz_primitive_v<T>);
    static_assert(is_dz_primitive_v<U>);

    using type = std::conditional_t<shell::is_any_of_v<dzfloat, T, U>, dzfloat, dzint>;
};

template<typename T, typename U>
using promoted_t = typename promoted<T, U>::type;

static_assert(std::is_same_v<promoted_t<dzbool, dzbool>, dzint>);
static_assert(std::is_same_v<promoted_t<dzbool, dzint>, dzint>);
static_assert(std::is_same_v<promoted_t<dzint, dzfloat>, dzfloat>);
