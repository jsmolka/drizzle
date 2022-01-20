#pragma once

#include "utils.h"

namespace tests_parser_expr_variable {

inline suite _ = [] {
  "parser_expr_variable"_test = [] {
    {
      constexpr auto kSource = R"(x)";
      constexpr auto kExpect = R"(program
  expression_statement
    variable x)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x = 1)";
      constexpr auto kExpect = R"(program
  expression_statement
    assign x
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x = 1 == 2)";
      constexpr auto kExpect = R"(program
  expression_statement
    assign x
      binary ==
        literal 1
        literal 2)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x = y == z)";
      constexpr auto kExpect = R"(program
  expression_statement
    assign x
      binary ==
        variable y
        variable z)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_expr_variable
