#pragma once

#include "utils.h"

namespace tests_parser_error {

inline suite _ = [] {
  "tests_parser_error"_test = [] {
    constexpr const char* kSources[] = {
R"(&0)",
R"(&&0)",
R"(!=0)",
R"(^0)",
R"(==0)",
R"(>0)",
R"(>>0)",
R"(>>>0)",
R"(>=0)",
R"(<0)",
R"(<<0)",
R"(<=0)",
R"(%0)",
R"(|0)",
R"(||0)",
R"(+0)",
R"(/0)",
R"(//0)",
R"(*0)",
R"(**0)",
R"(0!0)",
R"(0~0)",
R"(
if var x = 0:
  pass
)",
    };

    for (const auto& source : kSources) {
      parseThrows(source);
    }
  };
};

}  // namespace tests_parser_syntax
