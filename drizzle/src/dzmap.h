#pragma once

#include <optional>
#include <tsl/robin_map.h>

#include "dzobject.h"
#include "dzstring.h"
#include "dzvalue.h"

class DzMap : public DzObject {
public:
  DzMap();

  operator bool() const;
  auto operator==(const DzMap& other) const -> bool;
  auto repr() const -> std::string;

  auto get(const DzValue& key) const -> std::optional<DzValue>;
  void set(const DzValue& key, const DzValue& value);

  virtual auto subscriptGet(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value) override;

  tsl::robin_map<DzValue, DzValue> values;
};
