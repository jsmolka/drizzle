#pragma once

#include <sh/vector.h>

#include "dziterator.h"
#include "dzreverseiterator.h"
#include "dzvalue.h"

class DzList : public DzObject {
public:
  DzList();

  operator bool() const;
  auto operator==(const DzList& other) const -> bool;
  auto operator[](std::size_t index) -> DzValue&;
  auto operator[](std::size_t index) const -> const DzValue&;
  auto repr() const -> std::string;
  auto size() const -> std::size_t;

  sh::vector<DzValue> values;
};

class DzListIterator : public DzIterator {
public:
  DzListIterator(DzObject* iteree);

  virtual void advance() final;
  virtual auto dereference(Gc&) const -> DzValue final;

private:
  void set(std::size_t value);

  std::size_t index;
};

class DzListReverseIterator : public DzReverseIterator {
public:
  DzListReverseIterator(DzObject* iteree);

  virtual void advance() final;
  virtual auto dereference(Gc&) const -> DzValue final;

private:
  void set(std::size_t value);

  std::size_t index;
};
