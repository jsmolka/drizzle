#pragma once

#include <string_view>
#include <type_traits>

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
concept dz_object = std::conjunction_v<std::is_pointer<Ts>...> &&
    std::conjunction_v<std::is_base_of<DzObject, std::remove_pointer_t<Ts>>...>;
