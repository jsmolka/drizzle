#pragma once

#include <sh/vector.h>

#include "dziterator.h"

class DzBytes : public DzObject {
public:
  DzBytes();

  virtual operator bool() const override;
  virtual auto operator==(const DzObject& other) const -> bool override;
  auto operator[](std::size_t index) -> u8&;  // Todo: remove these
  auto operator[](std::size_t index) const -> const u8&;
  virtual auto size() const -> std::size_t override;
  virtual auto repr() const -> std::string override;

  virtual auto makeIterator(Vm& vm) -> DzValue override;
  virtual auto makeReverseIterator(Vm& vm) -> DzValue override;
  virtual auto getItem(Vm& vm, std::size_t index) -> DzValue override;
  virtual auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override;

  sh::vector<u8> data;

private:
  auto refExpr(Vm& vm, const DzValue& expr) -> u8&;
};
