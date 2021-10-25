#include "errors.h"

Error::Error(const std::string& message)
    : message(message)
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
