#pragma once

#include "dzfunction.h"
#include "dzstring.h"
#include "map.h"

class DzClass final : public DzObject {
public:
  static constexpr auto kInit = "init";

  DzClass(DzString* identifier);

  auto repr() const -> std::string override final;
  void mark(Gc& gc) override final;

  auto get(const DzString* identifier) const -> DzFunction*;
  void set(const DzString* identifier, DzFunction* function);

  DzString* identifier;
  DzFunction* init = nullptr;
  StringMap<DzFunction*> functions;
};
