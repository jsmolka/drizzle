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
      constexpr auto kSource = R"(
var x = [
])";
      constexpr auto kExpect = R"(program
  var x
    list)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
var x = [
  1, 2, 3
])";
      constexpr auto kExpect = R"(program
  var x
    list
      literal 1
      literal 2
      literal 3)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
var x = [
  1, 2, 3,
  4, 5, 6
])";
      constexpr auto kExpect = R"(program
  var x
    list
      literal 1
      literal 2
      literal 3
      literal 4
      literal 5
      literal 6)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
var x = [
  1,
  2,
  3
])";
      constexpr auto kExpect = R"(program
  var x
    list
      literal 1
      literal 2
      literal 3)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
var x = [
  1,

  2,

  3
])";
      constexpr auto kExpect = R"(program
  var x
    list
      literal 1
      literal 2
      literal 3)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
var x = [
  [
    1, 2,
    3
  ],
  [],
  [
  ]
])";
      constexpr auto kExpect = R"(program
  var x
    list
      list
        literal 1
        literal 2
        literal 3
      list
      list)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
R"([)",
R"([1, ])",
R"([1, if])",
R"(
block:
  var x = [
])",
R"(
var x = [
  [
]
])",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // tests_parser_expr_list
