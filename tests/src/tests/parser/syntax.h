#pragma once

#include "utils.h"

namespace tests_parser_syntax {

inline suite _ = [] {
  "tests_parser_syntax"_test = [] {
    constexpr const char* kSources[] = {
      "&0",
      "&&0",
      "!=0",
      "^0",
      "==0",
      ">0",
      ">>0",
      ">>>0",
      ">=0",
      "<0",
      "<<0",
      "<=0",
      "%0",
      "|0",
      "||0",
      "+0",
      "/0",
      "//0",
      "*0",
      "**0",
      "0!0",
      "0~0",
      R"(
if var x = 0:
  pass)"
    };

    for (const auto& source : kSources) {
      parseThrows(source);
    }
  };
};

}  // namespace tests_parser_syntax
