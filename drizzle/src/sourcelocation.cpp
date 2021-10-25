#include "sourcelocation.h"

SourceLocation::SourceLocation(const char* location)
    : type(Type::Location), location(location) 
{
}

SourceLocation::SourceLocation(std::string_view location)
    : SourceLocation(location.data())
{
}

SourceLocation::SourceLocation(std::size_t line)
    : type(Type::Line), line(line)
{
}
