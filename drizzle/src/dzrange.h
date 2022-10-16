#pragma once

#include "dziterator.h"
#include "dzobject.h"
#include "dzprimitives.h"

class DzRange : public DzObject {
public:
  DzRange(dzint start, dzint stop, dzint step);

  virtual auto operator==(const DzObject& other) const -> bool;
  virtual auto repr() const -> std::string override;

  dzint start;
  dzint stop;
  dzint step;
};

class DzRangeIterator : public DzIterator {
public:
  DzRangeIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc&) const -> DzValue final;

private:
  dzint iter;
};

class DzRangeReverseIterator : public DzIterator {
public:
  DzRangeReverseIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc&) const -> DzValue final;

private:
  dzint iter;
  dzint step;
  dzint stop;
};

