#pragma once

#include "utils.h"

namespace tests_parser_stmt_switch {

inline suite _ = [] {
  "parser_stmt_switch"_test = [] {
    {
      constexpr auto kSource = R"(
switch 1:
  case 1:
    noop
)";
      constexpr auto kExpect = R"(program
  switch
    literal 1
    case
      literal 1
      noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
switch 1 + 1:
  case 1 + 1:
    noop
)";
      constexpr auto kExpect = R"(program
  switch
    binary +
      literal 1
      literal 1
    case
      binary +
        literal 1
        literal 1
      noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
switch 0:
  case 1:
    noop
  case 2:
    noop
  default:
    noop
)";
      constexpr auto kExpect = R"(program
  switch
    literal 0
    case
      literal 1
      noop
    case
      literal 2
      noop
    default
      noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
R"(
switch
)",
R"(
switch:
)",
R"(
switch:
  default:
    noop
)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // tests_parser_stmt_switch
