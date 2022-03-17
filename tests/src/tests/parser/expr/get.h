#pragma once

#include "utils.h"

namespace tests_parser_expr_get {

inline suite _ = [] {
  "parser_expr_get"_test = [] {
    {
      constexpr auto kSource = R"(x.y)";
      constexpr auto kExpect = R"(program
  expression_statement
    get y
      variable x)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y.z)";
      constexpr auto kExpect = R"(program
  expression_statement
    get z
      get y
        variable x)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        R"(x.)",
        R"(x."test")",
        R"(x.0)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_expr_get
