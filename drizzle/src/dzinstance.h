#pragma once

#include <optional>

#include "dzclass.h"
#include "dzvalue.h"

class DzInstance : public DzObject {
public:
  DzInstance(DzClass* class_);

  operator bool() const;
  auto repr() const -> std::string;

  auto get(DzString* identifier) -> std::optional<DzValue>;
  void set(DzString* identifier, const DzValue& value);

  virtual auto subscriptGet(Vm& vm, const DzValue& expr) -> std::optional<DzValue> final;

  DzClass* class_;
  Map<DzValue> fields;
};
