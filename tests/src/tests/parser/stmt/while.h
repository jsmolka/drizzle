#pragma once

#include "utils.h"

namespace tests_parser_stmt_while {

inline suite _ = [] {
  "parser_stmt_while"_test = [] {
    {
      constexpr auto kSource = R"(
while true:
  noop)";
      constexpr auto kExpect = R"(program
  while
    literal true
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
while 1 == 1:
  noop)";
      constexpr auto kExpect = R"(program
  while
    binary ==
      literal 1
      literal 1
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        R"(
while:
  noop)",
        R"(
while true
  noop)",
        R"(while true: noop)",
        R"(while true:)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_stmt_while
