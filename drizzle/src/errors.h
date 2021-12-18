#pragma once

#include <stdexcept>

#include <sh/fmt.h>

class Error : public std::exception {
 public:
  class Location {
   public:
    enum class Type { None, Line, Location };

    Location();
    Location(std::size_t line);
    Location(const char* location);

    const Type type;
    union {
      const std::size_t line;
      const char* const location;
    };
  };

  template <typename... Args>
  Error(const Location& location, std::string_view format, Args&&... args)
      : location(location),
        message(fmt::format(fmt::runtime(format), std::forward<Args>(args)...)) {}

  virtual const char* name() const noexcept = 0;
  virtual const char* what() const noexcept final;

  const Location location;

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
