#pragma once

#include "utils.h"

namespace tests_parser_expr_in {

inline suite _ = [] {
  "parser_expr_in"_test = [] {
    {
      constexpr auto kSource = R"(1 in [1])";
      constexpr auto kExpect = R"(program
  expression_statement
    in
      list
        literal 1
      literal 1)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_expr_in
