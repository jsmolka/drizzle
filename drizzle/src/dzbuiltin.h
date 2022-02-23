#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "dzobject.h"

class Vm;

class DzBuiltIn : public DzObject {
public:
  using Callback = std::function<void(Vm&, std::size_t)>;

  static std::vector<DzBuiltIn> all;

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string_view identifier;
  std::optional<std::size_t> arity;
  Callback callback;

private:
  DzBuiltIn(std::string_view identifier, std::optional<std::size_t> arity, const Callback& callback);
};
