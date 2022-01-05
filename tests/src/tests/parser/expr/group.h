#pragma once

#include "utils.h"

namespace tests_parser_expr_group {

inline suite _ = [] {
  "parser_expr_group"_test = [] {
    {
      constexpr auto kSource = "(1 * 2) + (3 * 4)\n";
      parse(kSource, R"(program
  expression_statement
    binary +
      binary *
        literal 1
        literal 2
      binary *
        literal 3
        literal 4)");
    }
    {
      constexpr auto kSource = "1 * (2 + 3) * 4\n";
      parse(kSource, R"(program
  expression_statement
    binary *
      binary *
        literal 1
        binary +
          literal 2
          literal 3
      literal 4)");
    }
  };
};

}  // namespace tests_parser_expr_group
