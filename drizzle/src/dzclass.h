#pragma once

#include <string>

#include "dzmethod.h"
#include "dzobject.h"

class DzClass : public DzObject {
public:
  DzClass(const std::string& identifier);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string identifier;
  std::vector<DzFunction*> methods;
};
