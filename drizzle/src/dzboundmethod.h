#pragma once

#include "dzfunction.h"

class DzBoundMethod : public DzObject {
public:
  DzBoundMethod(DzObject* self, DzFunction* function);

  operator bool() const;
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;

  DzObject* self;
  DzFunction* function;
};
