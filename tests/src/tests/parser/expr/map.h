#pragma once

#include "utils.h"

namespace tests_parser_expr_map {

inline suite _ = [] {
  "parser_expr_map"_test = [] {
    {
      constexpr auto kSource = R"({})";
      constexpr auto kExpect = R"(program
  expression_statement
    map)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"({"key": 1})";
      constexpr auto kExpect = R"(program
  expression_statement
    map
      literal "key"
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"({"a" + "b": 1})";
      constexpr auto kExpect = R"(program
  expression_statement
    map
      binary +
        literal "a"
        literal "b"
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"({"key": {"key": 1}})";
      constexpr auto kExpect = R"(program
  expression_statement
    map
      literal "key"
      map
        literal "key"
        literal 1)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_expr_map
