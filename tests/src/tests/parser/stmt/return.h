#pragma once

#include "utils.h"

namespace tests_parser_stmt_return {

inline suite _ = [] {
  "parser_stmt_return"_test = [] {
    {
      constexpr auto kSource = R"(return)";
      constexpr auto kExpect = R"(program
  return
    literal null)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(return 1 + 1)";
      constexpr auto kExpect = R"(program
  return
    binary +
      literal 1
      literal 1)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
        R"(return return)",
        R"(return var x = 1)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // tests_parser_stmt_return
