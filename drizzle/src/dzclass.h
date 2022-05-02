#pragma once

#include <string>

#include "dzmethod.h"
#include "dzstring.h"

class DzClass : public DzObject {
public:
  static constexpr auto kInit = "init";

  DzClass(DzString* identifier);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  DzString* identifier;
  std::vector<DzFunction*> methods;
};
