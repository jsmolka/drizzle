#pragma once

#include "utils.h"

namespace tests_parser_stmt_expression_statement {

inline suite _ = [] {
  "parser_stmt_expression_statement"_test = [] {
    {
      constexpr auto kSource = R"(null)";
      constexpr auto kExpect = R"(program
  expression_statement
    literal null)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_stmt_expression_statement
