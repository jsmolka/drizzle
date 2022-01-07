#pragma once

#include "utils.h"

namespace tests_parser_stmt_block {

inline suite _ = [] {
  "parser_stmt_block"_test = [] {
    {
      constexpr auto kSource = R"(
block:
  noop
  noop)";
      constexpr auto kExpect = R"(program
  block
    noop
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
block name:
  noop
  noop)";
      constexpr auto kExpect = R"(program
  block name
    noop
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
block name
  noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
block
  noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
block: noop)";
      parseThrows(kSource);
    }
  };
};

}  // tests_parser_stmt_block
