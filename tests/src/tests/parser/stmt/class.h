#pragma once

#include "utils.h"

namespace tests_parser_stmt_class {

inline suite _ = [] {
  "parser_stmt_class"_test = [] {
    {
      constexpr auto kSource = R"(
class Test:
  def test():
    noop
)";
      constexpr auto kExpect = R"(program
  class Test
    def test
      noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
class Test:
  noop
)";
      constexpr auto kExpect = R"(program
  class Test
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
R"(
class Test
  noop
)",
R"(
class Test:
)",
R"(
class Test:
  if true:
    noop
)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_stmt_class
