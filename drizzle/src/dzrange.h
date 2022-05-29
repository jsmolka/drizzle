#pragma once

#include "dziterator.h"
#include "dzobject.h"
#include "dzprimitives.h"

class DzRange : public DzObject {
public:
  DzRange(dzint start, dzint stop, dzint step);

  operator bool() const;
  auto operator==(const DzRange& other) const -> bool;
  auto repr() const -> std::string;

  dzint start;
  dzint stop;
  dzint step;
};

class DzRangeIterator : public DzIterator {
public:
  DzRangeIterator(DzObject* iteree);

  virtual void advance() final;
  virtual auto dereference(Gc&) const -> DzValue final;

private:
  void set(dzint value);

  dzint value;
};
