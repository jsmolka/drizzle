#pragma once

#include "dzobject.h"

class DzNull : public DzObject {
public:
  DzNull();

  virtual operator bool() const override;
  virtual auto repr() const -> std::string override;
};

inline DzNull null;
