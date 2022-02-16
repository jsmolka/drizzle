#pragma once

#include <array>
#include <functional>
#include <optional>

#include "dzobject.h"

class Vm;

class DzBuiltIn : public DzObject {
public:
  using Callback = std::function<void(Vm&, std::size_t)>;
  using BuiltIns = std::array<DzBuiltIn, 3>;

  DzBuiltIn(const std::string& identifier, std::optional<std::size_t> arity, const Callback& callback);

  static auto all() -> BuiltIns&;

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string identifier;
  std::optional<std::size_t> arity;
  Callback callback;
};
