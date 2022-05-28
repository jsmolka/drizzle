#pragma once

#include "dziterator.h"
#include "dzstring.h"

class DzStringIterator : public DzIterator {
public:
  DzStringIterator(DzObject* iteree);

  virtual void increment() final;
  virtual auto dereference(Gc& gc) const -> DzValue final;

private:
  auto string() const -> const DzString*;

  std::size_t index = 0;
};

