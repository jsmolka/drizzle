#pragma once

#include "dziterator.h"

class DzListIterator : public DzIterator {
public:
  virtual operator bool() const final;

  virtual void next() final;
  virtual auto value() const -> DzValue final;

  std::size_t index = 0;
};
