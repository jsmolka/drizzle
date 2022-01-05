#pragma once

#include "utils.h"

namespace tests_parser_stmt_continue {

inline suite _ = [] {
  "parser_stmt_continue"_test = [] {
    {
      constexpr auto kSource = R"(
continue
)";
      parse(kSource, R"(program
  continue)");
    }
    {
      constexpr auto kSource = R"(
while true:
  continue
)";
      parse(kSource, R"(program
  while
    literal true
    continue)");
    }
    {
      constexpr auto kSource = "continue";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_stmt_continue
