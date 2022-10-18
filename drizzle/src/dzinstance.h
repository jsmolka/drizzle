#pragma once

#include <optional>

#include "dzclass.h"
#include "dzvalue.h"

class DzInstance : public DzObject {
public:
  DzInstance(DzClass* class_);

  virtual auto repr() const -> std::string override;

  auto get(DzString* identifier) const -> std::optional<DzValue>;
  void set(DzString* identifier, const DzValue& value);

  virtual auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override;

  DzClass* class_;
  Map<DzValue> fields;
};
