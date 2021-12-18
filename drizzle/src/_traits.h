#pragma once

#include <type_traits>
#include <utility>

namespace shell {

template <typename T, typename... Ts>
inline constexpr bool is_any_of_v = std::disjunction_v<std::is_same<T, Ts>...>;

template <typename T, typename... Ts>
struct is_any_of : std::bool_constant<is_any_of_v<T, Ts...>> {};

template <typename T, typename... Ts>
inline constexpr bool is_same_v = std::conjunction_v<std::is_same<T, Ts>...>;

template <typename T, typename... Ts>
struct is_same : std::bool_constant<is_same_v<T, Ts...>> {};

template <typename T, template <typename> typename Func, typename = std::void_t<>>
inline constexpr bool is_detected_v = false;

template <typename T, template <typename> typename Func>
inline constexpr bool is_detected_v<T, Func, std::void_t<Func<T>>> = true;

template <typename T, template <typename> typename Func>
struct is_detected : std::bool_constant<is_detected_v<T, Func>> {};

template <typename T, template <typename...> typename Template>
inline constexpr bool is_specialization_v = false;

template <template <typename...> typename Template, typename... Ts>
inline constexpr bool is_specialization_v<Template<Ts...>, Template> = true;

template <typename T, template <typename...> typename Template>
struct is_specialization : std::bool_constant<is_specialization_v<T, Template>> {};

template <typename T, bool = std::is_enum<T>::value>
struct is_scoped_enum : std::false_type {};

template <typename T>
struct is_scoped_enum<T, true>
    : std::integral_constant<bool, !std::is_convertible_v<T, std::underlying_type_t<T>>> {};

template <typename T>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

template <typename T>
struct unqualified {
  using type = std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>;
};

template <typename T>
using unqualified_t = typename unqualified<T>::type;

}  // namespace shell
