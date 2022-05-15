#pragma once

#include <vector>

#include "dzclass.h"

class DzList : public DzObject {
public:
  DzList(DzClass* class_);

  operator bool() const;
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;

  DzClass* class_;
  std::vector<DzValue> values;
};
