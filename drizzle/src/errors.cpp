#include "errors.h"

Error::Location::Location()
    : type(Type::None)
{
}

Error::Location::Location(std::size_t line)
    : type(Type::Line), line(line)
{
}

Error::Location::Location(const char* location)
    : type(Type::Location), location(location)
{
}

const char* Error::what() const noexcept
{
    return message.c_str();
}

const char* SyntaxError::name() const noexcept
{
    return "SyntaxError";
}
