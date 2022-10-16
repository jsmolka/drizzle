#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzvalue.h"

class Vm;

class DzObject {
public:
  enum class Type {
    BoundMethod,
    Bytes,
    Class,
    Function,
    Instance,
    Iterator,
    List,
    Map,
    Null,
    Range,
    SdlWindow,
    String,
    LastEnumValue
  };

  DzObject(Type type);
  virtual ~DzObject() = default;

  virtual operator bool() const;
  virtual auto operator==(const DzObject& other) const -> bool;
  virtual auto hash() const -> std::size_t;
  virtual auto repr() const -> std::string = 0;
  auto kind() const -> std::string_view;

  template<typename T>
    requires std::is_base_of_v<DzObject, T>
  auto as() -> T* {
    assert(this);
    return static_cast<T*>(this);
  }
  template<typename T>
    requires std::is_base_of_v<DzObject, T>
  auto as() const -> const T* {
    assert(this);
    return static_cast<const T*>(this);
  }
  auto is(Type type) const -> bool;

  virtual auto subscriptGet(Vm& vm, const DzValue& expr) -> DzValue;
  virtual void subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value);

  Type type;
  bool marked;
  DzObject* next;
};

template<typename... Ts>
concept dz_object =
    std::conjunction_v<std::is_pointer<Ts>...> &&
    std::conjunction_v<std::is_base_of<DzObject, std::remove_pointer_t<Ts>>...>;

template<>
struct fmt::formatter<DzObject::Type> : fmt::formatter<std::string_view> {
  static auto repr(const DzObject::Type& type) -> std::string_view {
    static_assert(int(DzObject::Type::LastEnumValue) == 12);
    switch (type) {
      case DzObject::Type::Bytes:       return "bytes";
      case DzObject::Type::BoundMethod: return "function";
      case DzObject::Type::Class:       return "class";
      case DzObject::Type::Function:    return "function";
      case DzObject::Type::Instance:    return "instance";
      case DzObject::Type::Iterator:    return "iterator";
      case DzObject::Type::List:        return "list";
      case DzObject::Type::Map:         return "map";
      case DzObject::Type::Null:        return "null";
      case DzObject::Type::Range:       return "range";
      case DzObject::Type::SdlWindow:   return "instance";
      case DzObject::Type::String:      return "string";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }
  
  template<typename FormatContext>
  auto format(const DzObject::Type& type, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(repr(type), ctx);
  }
};

namespace std {

template<>
struct hash<DzObject> {
  auto operator()(const DzObject& object) const -> std::size_t {
    return object.hash();
  }
};

}  // namespace std

