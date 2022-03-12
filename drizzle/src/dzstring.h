#pragma once

#include "dzobject.h"

class DzString : public DzObject {
public:
  DzString(const std::string& data);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string data;
  std::size_t hash;
};
