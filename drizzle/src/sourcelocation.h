#pragma once

#include <cstddef>

struct SourceLocation {
  // Todo: find optimal types
  int line = 0;
  int column = 0;
};
