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

  virtual auto makeIterator(Vm& vm) -> DzValue override;
  virtual auto makeReverseIterator(Vm& vm) -> DzValue override;
  virtual auto getAt(Vm& vm, std::size_t index) -> DzValue override;
  virtual auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override;

  sh::vector<DzValue> values;

private:
  auto refExpr(Vm& vm, const DzValue& expr) -> DzValue&;
};
