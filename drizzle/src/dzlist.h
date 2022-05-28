#pragma once

#include <sh/vector.h>

#include "dziterator.h"
#include "dzobject.h"
#include "dzvalue.h"

class DzList : public DzObject {
public:
  DzList();

  operator bool() const;
  auto operator==(const DzList& other) const -> bool;
  auto repr() const -> std::string;

  sh::vector<DzValue> values;
};

class DzListIterator : public DzIterator {
public:
  DzListIterator(DzObject* iteree);

  virtual void increment() final;
  virtual auto dereference(Gc&) const -> DzValue final;

private:
  auto list() const -> const DzList*;

  std::size_t index = 0;
};
