#pragma once

#include <vector>

#include "dzobject.h"
#include "dzvalue.h"

class DzList : public DzObject {
public:
  DzList();

  operator bool() const;
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;

  std::vector<DzValue> values;
};
