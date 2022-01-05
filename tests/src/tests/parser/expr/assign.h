#pragma once

#include "utils.h"

namespace tests_parser_expr_assign {

inline suite _ = [] {
  "parser_expr_assign"_test = [] {
    {
      constexpr auto kSource = "x\n";
      parse(kSource, R"(program
  expression_statement
    variable x)");
    }
    {
      constexpr auto kSource = "x == y\n";
      parse(kSource, R"(program
  expression_statement
    binary ==
      variable x
      variable y)");
    }
    {
      constexpr auto kSource = "var x = y = 1\n";
      parse(kSource, R"(program
  var x
    assign y
      literal 1)");
    }
    {
      constexpr auto kSource = "var x = 1 && 1 && y = 1\n";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_expr_assign
