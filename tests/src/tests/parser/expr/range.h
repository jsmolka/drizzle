#pragma once

#include "utils.h"

namespace tests_parser_expr_range {

inline suite _ = [] {
  "parser_expr_range"_test = [] {
    {
      constexpr auto kSource = R"(0 .. 1)";
      constexpr auto kExpect = R"(program
  expression_statement
    range
      literal 0
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(0 + 1 .. 1 + 2)";
      constexpr auto kExpect = R"(program
  expression_statement
    range
      binary +
        literal 0
        literal 1
      binary +
        literal 1
        literal 2)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(0 | 1 .. x.size())";
      constexpr auto kExpect = R"(program
  expression_statement
    range
      binary |
        literal 0
        literal 1
      invoke size
        variable x)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_expr_range
