#pragma once

#include <vector>

#include "dzobject.h"
#include "dzvalue.h"

class DzList : public DzObject {
public:
  DzList();

  operator bool() const;
  auto repr() const -> std::string;

  std::vector<DzValue> values;
};
