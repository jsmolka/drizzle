#pragma once

#include "utils.h"

namespace tests_parser_expr_group {

inline suite _ = [] {
  "parser_expr_group"_test = [] {
    {
      constexpr auto kSource = R"((1 * 2) + (3 * 4))";
      constexpr auto kExpect = R"(program
  expression_statement
    binary +
      binary *
        literal 1
        literal 2
      binary *
        literal 3
        literal 4)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(1 * (2 + 3) * 4)";
      constexpr auto kExpect = R"(program
  expression_statement
    binary *
      binary *
        literal 1
        binary +
          literal 2
          literal 3
      literal 4)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        "(",
        "(()",
        "((())",
        "(1",
        "((1)",
        "(((1))",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_expr_group
