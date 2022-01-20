#pragma once

#include "utils.h"

namespace tests_parser_stmt_var {

inline suite _ = [] {
  "parser_stmt_var"_test = [] {
    {
      constexpr auto kSource = R"(var x = 0)";
      constexpr auto kExpect = R"(program
  var x
    literal 0)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(var x)";
      constexpr auto kExpect = R"(program
  var x
    literal null)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
var x
var y)";
      constexpr auto kExpect = R"(program
  var x
    literal null
  var y
    literal null)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        R"(var)",
        R"(var x =)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // namespace tests_parser_stmt_var
