#pragma once

#include "utils.h"

namespace tests_parser_stmt_for {

inline suite _ = [] {
  "parser_stmt_expression_for"_test = [] {
    {
      constexpr auto kSource = R"(
for x in [1, 2, 3]:
  noop
)";
      constexpr auto kExpect = R"(program
  for x
    list
      literal 1
      literal 2
      literal 3
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
R"(
for in []:
  noop
)",
R"(
for x []:
  noop
)",
R"(
for x in:
  noop
)",
R"(
for x in []: noop
)",
R"(
for x in []:
)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_stmt_for
