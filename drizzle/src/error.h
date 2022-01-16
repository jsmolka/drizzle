#pragma once

#include <sh/error.h>

#include "location.h"

class Error : public sh::error {
public:
  template<typename... Args>
  Error(const Location& location, std::string_view format, Args&&... args)
    : error(format, std::forward<Args>(args)...), location(location) {}

  virtual auto name() const -> std::string_view = 0;

  Location location;
};

class SyntaxError : public Error {
public:
  using Error::Error;

  auto name() const -> std::string_view final {
    return "SyntaxError";
  }
};

class CompilerError : public Error {
public:
  using Error::Error;

  auto name() const -> std::string_view final {
    return "CompilerError";
  }
};

class RuntimeError : public Error {
public:
  using Error::Error;

  auto name() const -> std::string_view final {
    return "RuntimeError";
  }
};
