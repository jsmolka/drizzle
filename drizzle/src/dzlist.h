#pragma once

#include <sh/vector.h>

#include "dziterator.h"
#include "dzvalue.h"

class DzList : public DzObject {
public:
  DzList();

  virtual operator bool() const override;
  virtual auto operator==(const DzObject& other) const -> bool override;
  auto operator[](std::size_t index) -> DzValue&;
  auto operator[](std::size_t index) const -> const DzValue&;
  virtual auto size() const -> std::size_t override;
  virtual auto repr() const -> std::string override;

  virtual auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override;

  sh::vector<DzValue> values;

private:
  auto subscript(Vm& vm, const DzValue& expr) -> DzValue&;
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
