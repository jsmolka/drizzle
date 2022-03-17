#pragma once

#include "dzclass.h"

class DzInstance : public DzObject {
public:
  DzInstance(DzClass* class_);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  DzClass* class_;
};
