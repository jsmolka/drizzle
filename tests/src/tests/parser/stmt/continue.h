#pragma once

#include "utils.h"

namespace tests_parser_stmt_continue {

inline suite _ = [] {
  "parser_stmt_continue"_test = [] {
    {
      constexpr auto kSource = R"(continue)";
      constexpr auto kExpect = R"(program
  continue)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
while true:
  continue)";
      constexpr auto kExpect = R"(program
  while
    literal true
    continue)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_stmt_continue
