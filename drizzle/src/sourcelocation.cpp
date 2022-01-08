#include "sourcelocation.h"

SourceLocation::SourceLocation(std::size_t line)
  : line(line), column(0) {}
