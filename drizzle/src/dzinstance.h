#pragma once

#include <tsl/robin_map.h>

#include "dzclass.h"
#include "dzstring.h"
#include "dzvalue.h"

class DzInstance : public DzObject {
public:
  DzInstance(DzClass* class_);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  auto get(DzString* name) -> DzValue;
  void set(DzString* name, const DzValue& value);

  DzClass* class_;
  tsl::robin_map<DzString*, DzValue, DzString::Hash, DzString::Equal> fields;
};
