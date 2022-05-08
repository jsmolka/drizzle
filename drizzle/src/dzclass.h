#pragma once

#include <optional>

#include "dzfunction.h"
#include "dzstring.h"
#include "map.h"

class DzClass : public DzObject {
public:
  static constexpr auto kInit = "init";

  DzClass(DzString* identifier);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;
  void add(DzFunction* function);
  auto get(DzString* identifier) -> DzFunction*;

  DzString* identifier;
  DzFunction* init = nullptr;
  Map<DzFunction*> functions;
};
