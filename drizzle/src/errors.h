#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <shell/errors.h>

class Error : public shell::Error
{
public:
    using shell::Error::Error;

    virtual std::string_view name() const noexcept = 0;
};

class LocationError : public Error
{
public:
    template<typename... Args>
    LocationError(const char* location, std::string_view message, Args&&... args)
        : Error(message, std::forward<Args>(args)...), location(location) {}

    const char* const location;
};

class LineError : public Error
{
public:
    template<typename... Args>
    LineError(std::size_t line, std::string_view message, Args&&... args)
        : Error(message, std::forward<Args>(args)...), line(line) {}

    const std::size_t line;
};

class CompilerError : public Error
{
public:
    using Error::Error;

    virtual std::string_view name() const noexcept final
    {
        return "CompilerError";
    }
};

class SyntaxError : public LocationError
{
public:
    using LocationError::LocationError;

    virtual std::string_view name() const noexcept final
    {
        return "SyntaxError";
    }
};

class RuntimeError : public LineError
{
public:
    using LineError::LineError;
};

class TypeError : public RuntimeError
{
public:
    using RuntimeError::RuntimeError;

    virtual std::string_view name() const noexcept final
    {
        return "TypeError";
    }
};

class ZeroDivsionError : public RuntimeError
{
public:
    using RuntimeError::RuntimeError;

    virtual std::string_view name() const noexcept final
    {
        return "ZeroDivisionError";
    }
};
