#pragma once

#include "dziterator.h"
#include "dzlist.h"

class DzListIterator : public DzIterator {
public:
  DzListIterator(DzObject* iteree);

  virtual void increment() final;
  virtual auto dereference(Gc&) const -> DzValue final;

private:
  auto list() const -> const DzList*;

  std::size_t index = 0;
};
