#pragma once

#include "utils.h"

namespace tests_parser_stmt_noop {

inline suite _ = [] {
  "parser_stmt_noop"_test = [] {
    {
      constexpr auto kSource = R"(noop)";
      constexpr auto kExpect = R"(program
  noop)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_stmt_noop
