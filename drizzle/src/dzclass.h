#pragma once

#include "dzfunction.h"
#include "dzstring.h"
#include "map.h"

class DzClass : public DzObject {
public:
  static constexpr auto kInit = "init";

  DzClass(DzString* identifier);

  virtual auto repr() const -> std::string override;

  auto get(DzString* identifier) -> DzFunction*;
  void set(DzString* identifier, DzFunction* function);

  DzString* identifier;
  DzFunction* init = nullptr;
  Map<DzFunction*> functions;
};
