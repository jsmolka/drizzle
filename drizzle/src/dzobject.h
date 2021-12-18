#pragma once

#include <string_view>

#include "_traits.h"

class DzObject {
 public:
  enum class Type { String };

  DzObject(Type type);

  auto typeName() const -> std::string_view;

  operator bool() const;
  auto operator==(const DzObject& other) const -> bool;
  auto operator!=(const DzObject& other) const -> bool;

  const Type type;

 private:
  template <typename T>
    requires std::is_base_of_v<DzObject, T>
  auto as() const -> const T&;
};

template <typename... Ts>
inline constexpr auto is_dz_object_v = std::conjunction_v<std::is_pointer<Ts>...>&&
    std::conjunction_v<std::is_base_of<DzObject, shell::unqualified_t<Ts>>...>;

template <typename... Ts>
struct is_dz_object : std::bool_constant<is_dz_object_v<Ts...>> {};
