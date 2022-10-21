#pragma once

#include <sh/error.h>

#include "location.h"

class Error : public sh::error {
public:
  template<typename... Args>
  Error(const Location& location, Args&&... args)
    : error(std::forward<Args>(args)...), location(location) {}

  virtual auto name() const -> std::string_view = 0;

  Location location;
};

class SyntaxError final : public Error {
public:
  using Error::Error;

  auto name() const -> std::string_view override final {
    return "SyntaxError";
  }
};

class CompilerError final : public Error {
public:
  using Error::Error;

  auto name() const -> std::string_view override final {
    return "CompilerError";
  }
};

class RuntimeError final : public Error {
public:
  using Error::Error;

  auto name() const -> std::string_view override final {
    return "RuntimeError";
  }
};
