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

private:
    std::string message;
};

class SyntaxError : public Error
{
public:
    SyntaxError(const std::string& message, const char* location)
        : Error(message), location(location) {}

    const char* location;
};

class RuntimeError : public Error
{
public:
    using Error::Error;
};
