#pragma once

#include "dzobject.h"

class DzIterator : public DzObject {
public:
  DzIterator(DzObject* iteree);

  virtual operator bool() const = 0;
  auto repr() const -> std::string;

  virtual void next() = 0;

  DzObject* iteree;
};
