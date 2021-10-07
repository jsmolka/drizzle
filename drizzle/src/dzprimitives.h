#pragma once

#include <shell/traits.h>

#include "int.h"

using dzint = s64;
using dzfloat = double;
using dzbool = bool;

template<typename... Ts>
inline constexpr auto is_dz_int_v = shell::is_same_v<dzint, Ts...>;

template<typename... Ts>
struct is_dz_int : std::bool_constant<is_dz_int_v<Ts...>> {};

template<typename... Ts>
inline constexpr auto is_dz_float_v = shell::is_same_v<dzfloat, Ts...>;

template<typename... Ts>
struct is_dz_float : std::bool_constant<is_dz_float_v<Ts...>> {};

template<typename... Ts>
inline constexpr auto is_dz_bool_v = shell::is_same_v<dzbool, Ts...>;

template<typename... Ts>
struct is_dz_bool : std::bool_constant<is_dz_bool_v<Ts...>> {};

template<typename... Ts>
inline constexpr auto is_dz_primitive_v = std::conjunction_v<shell::is_any_of<Ts, dzint, dzfloat, dzbool>...>;

template<typename... Ts>
struct is_dz_primitive : std::bool_constant<is_dz_primitive_v<Ts...>> {};

template<typename T, typename U>
struct promote
{
    static_assert(is_dz_primitive_v<T, U>);

    using type = std::conditional_t<shell::is_any_of_v<dzfloat, T, U>, dzfloat, dzint>;
};

template<typename T, typename U>
using promote_t = typename promote<T, U>::type;

static_assert(std::is_same_v<promote_t<dzbool, dzbool>, dzint>);
static_assert(std::is_same_v<promote_t<dzbool, dzint>, dzint>);
static_assert(std::is_same_v<promote_t<dzint, dzfloat>, dzfloat>);

template<typename T, typename U>
struct promote_lax
{
    static_assert(is_dz_primitive_v<T, U>);

    using type = std::conditional_t<shell::is_any_of_v<dzfloat, T, U>, dzfloat,
        std::conditional_t<shell::is_any_of_v<dzint, T, U>, dzint, dzbool>>;
};

template<typename T, typename U>
using promote_lax_t = typename promote_lax<T, U>::type;

static_assert(std::is_same_v<promote_lax_t<dzbool, dzbool>, dzbool>);
static_assert(std::is_same_v<promote_lax_t<dzbool, dzint>, dzint>);
static_assert(std::is_same_v<promote_lax_t<dzint, dzfloat>, dzfloat>);
