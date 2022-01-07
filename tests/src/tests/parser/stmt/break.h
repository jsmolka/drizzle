#pragma once

#include "utils.h"

namespace tests_parser_stmt_break {

inline suite _ = [] {
  "parser_stmt_break"_test = [] {
    {
      constexpr auto kSource = R"(break)";
      constexpr auto kExpect = R"(program
  break)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
while true:
  break)";
      constexpr auto kExpect = R"(program
  while
    literal true
    break)";
      parse(kSource, kExpect);
    }
  };
  {
    constexpr auto kSource = R"(break label)";
    constexpr auto kExpect = R"(program
  break label)";
    parse(kSource, kExpect);
  }
  {
    constexpr auto kSource = R"(
block:
  break label)";
    constexpr auto kExpect = R"(program
  block
    break label)";
    parse(kSource, kExpect);
  }
};

}  // namespace tests_parser_stmt_break
