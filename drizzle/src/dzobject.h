#pragma once

#include <sh/fmt.h>
#include <sh/utility.h>

#include <string>
#include <string_view>

class DzObject {
public:
  enum class Type { BoundMethod, Class, Function, Instance, List, Null, String, LastEnumValue };

  DzObject(Type type);
  virtual ~DzObject() = default;

  operator bool() const;
  auto operator==(const DzObject& other) const -> bool;
  auto operator!=(const DzObject& other) const -> bool;
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;

  auto is(Type type) const -> bool;

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

template<>
struct fmt::formatter<DzObject::Type> : fmt::formatter<std::string_view> {
  static_assert(int(DzObject::Type::LastEnumValue) == 7);
  template<typename FormatContext>
  auto format(const DzObject::Type& type, FormatContext& ctx) const {
    auto repr = [](const DzObject::Type& type) {
      switch (type) {
        case DzObject::Type::BoundMethod: return "function";
        case DzObject::Type::Class:       return "class";
        case DzObject::Type::Function:    return "function";
        case DzObject::Type::Instance:    return "class";
        case DzObject::Type::List:        return "list";
        case DzObject::Type::Null:        return "null";
        case DzObject::Type::String:      return "string";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(type), ctx);
  }
};
