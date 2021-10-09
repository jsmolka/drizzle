#pragma once

#include <string_view>

#include <shell/traits.h>

class DzObject
{
public:
    enum class Type { String };

    DzObject(Type type);

    std::string_view typeName() const;

    operator bool() const;
    bool operator==(const DzObject& other) const;
    bool operator!=(const DzObject& other) const;

    const Type type;

private:
    template<typename T>
    const T& as() const;
};

template<typename... Ts>
inline constexpr auto is_dz_object_v = std::conjunction_v<std::is_pointer<Ts>...>
    && std::conjunction_v<std::is_base_of<DzObject, shell::unqualified_t<Ts>>...>;

template<typename... Ts>
struct is_dz_object : std::bool_constant<is_dz_object_v<Ts...>> {};
