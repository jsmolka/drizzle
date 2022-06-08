#pragma once

#include "dzobject.h"
#include "dzvalue.h"

class Gc;

class DzIterator : public DzObject {
public:
  DzIterator(DzObject* iteree, std::string_view type);

  operator bool() const;
  auto repr() const -> std::string;

  virtual auto done() const -> bool = 0;
  virtual void advance() = 0;
  virtual auto current(Gc&) const -> DzValue = 0;

  mutable DzObject* iteree;

private:
  std::string_view type;
};
