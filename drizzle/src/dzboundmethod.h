#pragma once

#include "dzfunction.h"

class DzBoundMethod : public DzObject {
public:
  DzBoundMethod(DzObject* self, DzFunction* function);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  DzObject* self;
  DzFunction* function;
};
