#pragma once

#include <optional>

#include "dzobject.h"
#include "dzstring.h"
#include "dzvalue.h"
#include "map.h"

class DzMap : public DzObject {
public:
  DzMap();

  operator bool() const;
  auto operator==(const DzMap& other) const -> bool;
  auto repr() const -> std::string;

  auto get(DzString* identifier) -> std::optional<DzValue>;
  void set(DzString* identifier, const DzValue& value);

  Map<DzValue> values;
};
