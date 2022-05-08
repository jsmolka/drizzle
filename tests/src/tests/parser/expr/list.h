#pragma once

#include "utils.h"

namespace tests_parser_expr_list {

inline suite _ = [] {
  "parser_expr_list"_test = [] {
    {
      constexpr auto kSource = "[]";
      constexpr auto kExpect = R"(program
  expression_statement
    list)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = "[0, 1, 2]";
      constexpr auto kExpect = R"(program
  expression_statement
    list
      literal 0
      literal 1
      literal 2)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = "[0, 1, [2, 3]]";
      constexpr auto kExpect = R"(program
  expression_statement
    list
      literal 0
      literal 1
      list
        literal 2
        literal 3)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"([1, 0.5, "test"])";
      constexpr auto kExpect = R"(program
  expression_statement
    list
      literal 1
      literal 0.5
      literal "test")";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        "[",
        "[1, ]",
        "[1, if]",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // tests_parser_expr_list
