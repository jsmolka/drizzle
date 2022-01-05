#pragma once

#include "utils.h"

namespace tests_parser_stmt_noop {

inline suite _ = [] {
  "parser_stmt_noop"_test = [] {
    {
      constexpr auto kSource = "noop\n";
      parse(kSource, R"(program
  noop)");
    }
    {
      constexpr auto kSource = "noop";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_stmt_noop
