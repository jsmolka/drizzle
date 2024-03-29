#pragma once

#include <sh/vector.h>

#include "dzobject.h"

class DzBytes final : public DzObject {
public:
  DzBytes();
  DzBytes(const sh::vector<u8>& data);

  explicit operator bool() const override final;
  auto operator==(const DzObject& other) const -> bool override final;
  auto size() const -> std::size_t override final;
  auto repr() const -> std::string override final;

  auto makeIterator(Vm& vm) -> DzValue override final;
  auto makeReverseIterator(Vm& vm) -> DzValue override final;
  auto in(Vm& vm, const DzValue& value) -> bool override final;
  auto getItem(Vm& vm, std::size_t index) -> DzValue override final;
  auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override final;
  auto getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue override final;
  void setItem(Vm& vm, std::size_t index, const DzValue& value) override final;
  void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override final;

  sh::vector<u8> data;

private:
  static void defineMembers(Vm& vm);

  auto toIndex(Vm& vm, const DzValue& expr) const -> std::size_t;
  auto toInsertIndex(Vm& vm, const DzValue& expr) const -> std::size_t;
};
