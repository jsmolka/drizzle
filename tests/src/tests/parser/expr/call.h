#pragma once

#include "utils.h"

namespace tests_parser_expr_call {

inline suite _ = [] {
  "parser_expr_call"_test = [] {
    {
      constexpr auto kSource = R"(test(0))";
      constexpr auto kExpect = R"(program
  expression_statement
    call test
      literal 0)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(test(a, b, c))";
      constexpr auto kExpect = R"(program
  expression_statement
    call test
      variable a
      variable b
      variable c)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(test(1 + 1))";
      constexpr auto kExpect = R"(program
  expression_statement
    call test
      binary +
        literal 1
        literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(test(x = 1))";
      constexpr auto kExpect = R"(program
  expression_statement
    call test
      assign x
        literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        R"(test()",
        R"(test(x)",
        R"(test(x,)",
        R"(test(x,))",
        R"(test(noop))",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_expr_call
