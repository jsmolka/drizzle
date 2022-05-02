#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "dzobject.h"
#include "dzvalue.h"

class Vm;

class DzNativeFunction : public DzObject {
public:
  using Callback = std::function<DzValue(Vm&, std::size_t)>;

  static std::vector<DzNativeFunction> all;

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string_view identifier;
  std::optional<std::size_t> arity;
  Callback callback;

private:
  DzNativeFunction(std::string_view identifier, std::optional<std::size_t> arity, const Callback& callback);
};
