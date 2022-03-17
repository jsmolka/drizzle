#pragma once

#include <string>
#include <string_view>

class DzObject {
public:
  enum class Type { BuiltIn, Class, Function, Instance, Null, String, LastEnumValue };

  DzObject(Type type);
  virtual ~DzObject() = default;

  operator bool() const;
  auto operator==(const DzObject& other) const -> bool;
  auto operator!=(const DzObject& other) const -> bool;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  Type type;
  bool marked = false;
  DzObject* next = nullptr;

private:
  template<typename T>
  auto as() const -> const T&;
};

template<typename... Ts>
concept dz_object =
    std::conjunction_v<std::is_pointer<Ts>...> &&
    std::conjunction_v<std::is_base_of<DzObject, std::remove_pointer_t<Ts>>...>;
