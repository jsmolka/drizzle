#pragma once

#include "dzobject.h"
#include "dzvalue.h"

class Gc;

class DzReverseIterator : public DzObject {
public:
  DzReverseIterator(DzObject* iteree, std::string_view type);

  operator bool() const;
  auto repr() const -> std::string;

  virtual void advance() = 0;
  virtual auto dereference(Gc&) const -> DzValue = 0;

  DzObject* iteree;

private:
  std::string_view type;
};
