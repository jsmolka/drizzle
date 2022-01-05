#pragma once

#include "utils.h"

namespace tests_parser_stmt_program {

inline suite _ = [] {
  "parser_stmt_program"_test = [] {
    constexpr auto kSource = "null\nnull\nnull\n";
    parse(kSource, R"(program
  expression_statement
    literal null
  expression_statement
    literal null
  expression_statement
    literal null)");
  };
};

}  // namespace tests_parser_stmt_program
