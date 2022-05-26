#pragma once

#include "dziterator.h"

class DzListIterator : public DzIterator {
public:
  using DzIterator::DzIterator;

  virtual operator bool() const final;

  virtual void increment() final;
  virtual auto dereference(Gc&) const -> DzValue final;

  std::size_t index = 0;
};
