#pragma once

#include "dzobject.h"
#include "dzvalue.h"

class DzIterator : public DzObject {
public:
  DzIterator(DzObject* iteree);

  virtual operator bool() const = 0;
  auto repr() const -> std::string;

  virtual void next() = 0;
  virtual auto value() const -> DzValue = 0;

  DzObject* iteree;
};
