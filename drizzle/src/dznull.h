#pragma once

#include "dzobject.h"

class DzNull : public DzObject {
public:
  DzNull();

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;
};

inline static DzNull null;
