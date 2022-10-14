#pragma once

#include <optional>

#include "dzclass.h"
#include "dzvalue.h"

class DzInstance : public DzObject {
public:
  DzInstance(DzClass* class_);

  operator bool() const;
  auto repr() const -> std::string;

  auto get(DzString* identifier) const -> std::optional<DzValue>;
  void set(DzString* identifier, const DzValue& value);

  auto subscriptGet(Vm& vm, const DzValue& expr) -> DzValue;
  void subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value);

  DzClass* class_;
  Map<DzValue> fields;
};
