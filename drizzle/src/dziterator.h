#pragma once

#include "dzobject.h"
#include "dzvalue.h"

class Gc;

class DzIterator : public DzObject {
public:
  DzIterator(DzObject* iteree);

  operator bool() const;
  auto repr() const -> std::string;

  virtual void increment() = 0;
  virtual auto dereference(Gc&) const -> DzValue = 0;

  DzObject* iteree;
};
