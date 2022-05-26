#pragma once

#include "dzobject.h"
#include "dzvalue.h"

class DzIterator : public DzObject {
public:
  DzIterator(DzObject* iteree);

  virtual operator bool() const = 0;
  auto repr() const -> std::string;

  virtual void increment() = 0;
  virtual auto dereference() const -> DzValue = 0;

  DzObject* iteree;
};
