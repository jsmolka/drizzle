#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <shell/format.h>

class Error : public std::exception
{
public:
    Error(std::string_view message)
        : message(message) {}

    template<typename... Args>
    Error(std::string_view format, Args&&... args)
        : message(fmt::format(format, std::forward<Args>(args)...)) {}

    const char* what() const final
    {
        return message.c_str();
    }

private:
    std::string message;
};
