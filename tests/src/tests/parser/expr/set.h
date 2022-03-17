#pragma once

#include "utils.h"

namespace tests_parser_expr_set {

inline suite _ = [] {
  "parser_expr_set"_test = [] {
    {
      constexpr auto kSource = R"(x.y = 0)";
      constexpr auto kExpect = R"(program
  expression_statement
    set y
      variable x
      literal 0)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y.z = 0)";
      constexpr auto kExpect = R"(program
  expression_statement
    set z
      get y
        variable x
      literal 0)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(a + (x.y.z = 0))";
      constexpr auto kExpect = R"(program
  expression_statement
    binary +
      variable a
      set z
        get y
          variable x
        literal 0)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        R"(x. = 0)",
        R"(x."test" = 0)",
        R"(x.0 = 0)",
        R"(a + x.y.z = 0)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_expr_set
