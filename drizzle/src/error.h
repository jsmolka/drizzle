#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <shell/format.h>

class Error : public std::exception
{
public:
    template<typename... Args>
    Error(std::string_view format, Args&&... args)
        : message(fmt::format(format, std::forward<Args>(args)...)) {}

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
    using Error::Error;
};

class RuntimeError : public Error
{
public:
    using Error::Error;
};
