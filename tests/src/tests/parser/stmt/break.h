#pragma once

#include "utils.h"

namespace tests_parser_stmt_break {

inline suite _ = [] {
  "parser_stmt_break"_test = [] {
    {
      constexpr auto kSource = R"(
break
)";
      parse(kSource, R"(program
  break)");
    }
    {
      constexpr auto kSource = R"(
while true:
  break
)";
      parse(kSource, R"(program
  while
    literal true
    break)");
    }
  };
  {
    constexpr auto kSource = "break";
    parseThrows(kSource);
  }
  {
    constexpr auto kSource = R"(
break label
)";
    parse(kSource, R"(program
  break label)");
  }
  {
    constexpr auto kSource = R"(
block:
  break label
)";
    parse(kSource, R"(program
  block
    break label)");
  }
  {
    constexpr auto kSource = "break label";
    parseThrows(kSource);
  }
};

}  // namespace tests_parser_stmt_break
