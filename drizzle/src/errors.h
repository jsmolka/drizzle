#pragma once

#include <stdexcept>

#include <sh/fmt.h>

#include "sourcelocation.h"

class Error : public std::exception {
public:
  template <typename... Args>
  Error(const SourceLocation& location, std::string_view format, Args&&... args)
    : location(location), message(fmt::format(fmt::runtime(format), std::forward<Args>(args)...)) {}

  virtual const char* name() const noexcept = 0;
  virtual const char* what() const noexcept final;

  const SourceLocation location;

private:
  const std::string message;
};

class SyntaxError : public Error {
public:
  using Error::Error;

  const char* name() const noexcept final;
};

class CompilerError : public Error {
public:
  using Error::Error;

  const char* name() const noexcept final;
};

class RuntimeError : public Error {
public:
  using Error::Error;

  const char* name() const noexcept final;
};
