#pragma once

#include <stdexcept>
#include <shell/format.h>

class Error : public std::exception
{
public:
    class Location
    {
    public:
        enum class Type { None, Line, Location };

        Location();
        Location(std::size_t line);
        Location(const char* location);

        Type type;
        union
        {
            std::size_t line;
            const char* location;
        };
    };

    template<typename... Args>
    Error(const Location& location, std::string_view format, Args&&... args)
        : location(location), message(fmt::format(format, std::forward<Args>(args)...)) {}

    virtual const char* name() const noexcept = 0;
    virtual const char* what() const noexcept final;

    Location location;

private:
    std::string message;
};

class SyntaxError : public Error
{
public:
    using Error::Error;

    const char* name() const noexcept final;
};

class CompilerError : public Error
{
public:
    using Error::Error;

    const char* name() const noexcept final;
};

class RuntimeError : public Error
{
public:
    using Error::Error;

    const char* name() const noexcept final;
};
