#pragma once

#include <array>
#include <functional>

#include "dzobject.h"

class Vm;

class DzBuiltIn : public DzObject {
public:
  using Callback = std::function<void(Vm&)>;

  DzBuiltIn(const std::string& identifier, std::size_t arity, const Callback& callback);

  static auto all() -> std::array<DzBuiltIn, 1>&;

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string identifier;
  std::size_t arity;
  Callback callback;
};
