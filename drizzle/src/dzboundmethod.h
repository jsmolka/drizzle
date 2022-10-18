#pragma once

#include "dzfunction.h"

class DzBoundMethod final : public DzObject {
public:
  DzBoundMethod(DzObject* self, DzFunction* function);

  auto operator==(const DzObject& other) const -> bool override final;
  auto repr() const -> std::string override final;

  DzObject* self;
  DzFunction* function;
};
