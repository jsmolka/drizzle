#pragma once

#include <optional>

#include "dzclass.h"
#include "dzvalue.h"

class DzInstance final : public DzObject {
public:
  DzInstance(DzClass* class_);

  auto repr() const -> std::string override final;

  auto in(Vm& vm, const DzValue& value) -> bool;
  auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override final;
  auto getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue override final;
  void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override final;
  void setProp(Vm& vm, const DzValue& prop, const DzValue& value) override final;

  DzClass* class_;
  StringMap<DzValue> fields;
};
