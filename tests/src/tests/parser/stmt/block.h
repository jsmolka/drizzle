#pragma once

#include "utils.h"

namespace tests_parser_stmt_block {

inline suite _ = [] {
  "parser_stmt_block"_test = [] {
    {
      constexpr auto kSource = R"(
block:
  noop)";
      constexpr auto kExpect = R"(program
  block
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
block name:
  noop)";
      constexpr auto kExpect = R"(program
  block name
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
R"(
block name
  noop
)",
R"(
block
  noop
)",
R"(
block: noop
)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // tests_parser_stmt_block
