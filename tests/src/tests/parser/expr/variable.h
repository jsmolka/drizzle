#pragma once

#include "utils.h"

namespace tests_parser_expr_variable {

inline suite _ = [] {
  "parser_expr_variable"_test = [] {
    {
      constexpr auto kSource = "x = 1\n";
      parse(kSource, R"(program
  expression_statement
    assign x
      literal 1)");
    }
    {
      constexpr auto kSource = "x = 1 == 2\n";
      parse(kSource, R"(program
  expression_statement
    assign x
      binary ==
        literal 1
        literal 2)");
    }
    {
      constexpr auto kSource = "x = y == z\n";
      parse(kSource, R"(program
  expression_statement
    assign x
      binary ==
        variable y
        variable z)");
    }
  };
};

}  // namespace tests_parser_expr_variable
