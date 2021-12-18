#pragma once

#include <string>

#include "dzobject.h"

class DzString : public DzObject {
 public:
  DzString(std::string&& data, std::size_t hash);

  constexpr auto typeName() const -> std::string_view {
    return "string";
  }

  operator bool() const;

  const std::string data;
  const std::size_t hash;
};
