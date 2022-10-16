#pragma once

#include <string_view>
#include <vector>

#include "dzfunction.h"

struct Program {
  DzFunction* main = nullptr;
  std::vector<std::string_view> globals;
};
