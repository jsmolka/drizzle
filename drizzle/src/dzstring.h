#pragma once

#include "dzobject.h"

class DzString : public DzObject {
public:
  DzString(std::string&& data, std::size_t hash);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string data;
  std::size_t hash;
};
