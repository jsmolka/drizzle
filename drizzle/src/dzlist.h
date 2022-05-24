#pragma once

#include <sh/vector.h>

#include "dzobject.h"
#include "dzvalue.h"

class DzList : public DzObject {
public:
  DzList();

  operator bool() const;
  auto operator==(const DzList& other) const -> bool;
  auto repr() const -> std::string;

  sh::vector<DzValue> values;
};
