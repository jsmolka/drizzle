#pragma once

#include "dzobject.h"

class DzNull final : public DzObject {
public:
  DzNull();

  explicit operator bool() const override final;
  auto repr() const -> std::string override final;
};

inline DzNull null;
