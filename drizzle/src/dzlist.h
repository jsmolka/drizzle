#pragma once

#include <sh/vector.h>

#include "dziterator.h"
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

  virtual auto subscriptGet(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value) override;

  sh::vector<DzValue> values;
};

class DzListIterator : public DzIterator {
public:
  DzListIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc&) const -> DzValue final;

private:
  std::size_t index;
};

class DzListReverseIterator : public DzIterator {
public:
  DzListReverseIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc&) const -> DzValue final;

private:
  std::size_t index;
};
