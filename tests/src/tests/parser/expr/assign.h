#pragma once

#include "utils.h"

namespace tests_parser_expr_assign {

inline suite _ = [] {
  "parser_expr_assign"_test = [] {
    {
      constexpr auto kSource = R"(x)";
      constexpr auto kExpect = R"(program
  expression_statement
    variable x)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x == y)";
      constexpr auto kExpect = R"(program
  expression_statement
    binary ==
      variable x
      variable y)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(var x = y = 1)";
      constexpr auto kExpect = R"(program
  var x
    assign y
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(var x = 1 && 1 && y = 1)";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_expr_assign
