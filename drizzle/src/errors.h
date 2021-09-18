#pragma once

#include <stdexcept>
#include <string>

class Error : public std::exception
{
public:
    Error(const std::string& message)
        : message(message) {}

    const char* what() const noexcept final
    {
        return message.c_str();
    }

    virtual const char* name() const noexcept = 0;

private:
    const std::string message;
};

class LocationError : public Error
{
public:
    LocationError(const std::string& message, const char* location)
        : Error(message), location(location) {}

    const char* const location;
};

class LineError : public Error
{
public:
    LineError(const std::string& message, std::size_t line)
        : Error(message), line(line) {}

    const std::size_t line;
};

class CompilerError : public Error
{
public:
    using Error::Error;

    virtual const char* name() const noexcept final
    {
        return "CompilerError";
    }
};

class SyntaxError : public LocationError
{
public:
    SyntaxError(const std::string& message, const char* location)
        : LocationError(message, location) {}

    virtual const char* name() const noexcept final
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

    virtual const char* name() const noexcept final
    {
        return "TypeError";
    }
};

class ZeroDivsionError : public RuntimeError
{
public:
    using RuntimeError::RuntimeError;

    virtual const char* name() const noexcept final
    {
        return "ZeroDivisionError";
    }
};
