#pragma once

#include "utils.h"

namespace tests_parser_expr_invoke {

inline suite _ = [] {
  "parser_expr_invoke"_test = [] {
    {
      constexpr auto kSource = R"(x.y())";
      constexpr auto kExpect = R"(program
  expression_statement
    invoke y
      variable x)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y(0, 1))";
      constexpr auto kExpect = R"(program
  expression_statement
    invoke y
      variable x
      literal 0
      literal 1)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_expr_invoke
