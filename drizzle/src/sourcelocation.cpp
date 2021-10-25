#include "sourcelocation.h"

SourceLocation::SourceLocation(const char* position)
    : type(Type::Position), position(position) {}

SourceLocation::SourceLocation(std::string_view position)
    : SourceLocation(position.data()) {}

SourceLocation::SourceLocation(std::size_t line)
    : type(Type::Line), line(line) {}
