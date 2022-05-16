#pragma once

#include "dzfunction.h"
#include "dzstring.h"
#include "map.h"

class Gc;

class DzClass : public DzObject {
public:
  static constexpr auto kInit = "init";

  DzClass(DzString* identifier);

  operator bool() const;
  auto repr() const -> std::string;

  void add(DzFunction* function);
  auto get(DzString* identifier) -> DzFunction*;

  DzString* identifier;
  DzFunction* init = nullptr;
  Map<DzFunction*> functions;
};
