#pragma once

#include "utils.h"

namespace tests_parser_stmt_expression_statement {

inline suite _ = [] {
  "parser_stmt_expression_statement"_test = [] {
    {
      constexpr auto kSource = "null\n";
      parse(kSource, R"(program
  expression_statement
    literal null)");
    }
    {
      constexpr auto kSource = "null";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_stmt_expression_statement
