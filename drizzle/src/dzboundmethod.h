#pragma once

#include "dzfunction.h"

class DzBoundMethod : public DzObject {
public:
  DzBoundMethod(DzObject* self, DzFunction* function);

  virtual auto operator==(const DzObject& other) const -> bool override;
  virtual auto repr() const -> std::string override;

  DzObject* self;
  DzFunction* function;
};
