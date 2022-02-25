#pragma once

#include "utils.h"

namespace tests_parser_error {

inline suite _ = [] {
  "tests_parser_error"_test = [] {
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
    };

    for (const auto& source : kSources) {
      parseThrows(source);
    }
  };
};

}  // namespace tests_parser_syntax
