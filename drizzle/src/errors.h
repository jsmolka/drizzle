#pragma once

#include <stdexcept>
#include <shell/format.h>

#include "sourcelocation.h"

class Error : public std::exception
{
public:
    Error(const std::string& message);

    virtual const char* name() const noexcept = 0;
    virtual const char* what() const noexcept final;

private:
    std::string message;
};

class LocationError : public Error
{
public:
    template<typename... Args>
    LocationError(const SourceLocation& location, std::string_view format, Args&&... args)
        : Error(shell::format(format, std::forward<Args>(args)...)), location(location) {}

    const SourceLocation location;
};

class SyntaxError : public LocationError
{
public:
    using LocationError::LocationError;

    virtual const char* name() const noexcept final;
};
