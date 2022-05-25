#pragma once

#include "dzfunction.h"

class DzBoundMethod : public DzObject {
public:
  DzBoundMethod(DzObject* self, DzFunction* function);

  operator bool() const;
  auto operator==(const DzBoundMethod& other) const -> bool;
  auto repr() const -> std::string;

  DzObject* self;
  DzFunction* function;
};
