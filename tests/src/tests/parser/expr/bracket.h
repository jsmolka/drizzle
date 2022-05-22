#pragma once

#include "utils.h"

namespace tests_parser_expr_bracket {

inline suite _ = [] {
  "parser_expr_bracket"_test = [] {
    {
      constexpr auto kSource = R"(x[0])";
      constexpr auto kExpect = R"(program
  expression_statement
    bracket_get
      variable x
      literal 0)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y[0])";
      constexpr auto kExpect = R"(program
  expression_statement
    bracket_get
      get y
        variable x
      literal 0)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y[0 + 1])";
      constexpr auto kExpect = R"(program
  expression_statement
    bracket_get
      get y
        variable x
      binary +
        literal 0
        literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x[0] = 1)";
      constexpr auto kExpect = R"(program
  expression_statement
    bracket_set
      variable x
      literal 0
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y[0] = 1)";
      constexpr auto kExpect = R"(program
  expression_statement
    bracket_set
      get y
        variable x
      literal 0
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y[0 + 1] = 1)";
      constexpr auto kExpect = R"(program
  expression_statement
    bracket_set
      get y
        variable x
      binary +
        literal 0
        literal 1
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(x.y[0 + 1] = 1 + 2)";
      constexpr auto kExpect = R"(program
  expression_statement
    bracket_set
      get y
        variable x
      binary +
        literal 0
        literal 1
      binary +
        literal 1
        literal 2)";
      parse(kSource, kExpect);
    }
  };
};

}  // tests_parser_expr_bracket
