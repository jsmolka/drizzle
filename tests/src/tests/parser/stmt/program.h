#pragma once

#include "utils.h"

namespace tests_parser_stmt_program {

inline suite _ = [] {
  "parser_stmt_program"_test = [] {
    constexpr auto kSource = R"(
null
null
null)";
    constexpr auto kExpect = R"(program
  expression_statement
    literal null
  expression_statement
    literal null
  expression_statement
    literal null)";
    parse(kSource, kExpect);
  };
};

}  // namespace tests_parser_stmt_program