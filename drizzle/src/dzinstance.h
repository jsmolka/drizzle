#pragma once

#include <optional>

#include "dzclass.h"
#include "dzvalue.h"

class DzInstance : public DzObject {
public:
  DzInstance(DzClass* class_);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;
  auto get(DzString* identifier) -> std::optional<DzValue>;
  void set(DzString* identifier, const DzValue& value);

  DzClass* class_;
  Map<DzValue> fields;
};
