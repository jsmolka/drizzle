#pragma once

#include "utils.h"

namespace tests_parser_stmt_program {

inline suite _ = [] {
  "parser_stmt_program"_test = [] {
    constexpr auto kSource = R"(
noop
noop
noop)";
    constexpr auto kExpect = R"(program
  noop
  noop
  noop)";
    parse(kSource, kExpect);
  };
};

}  // namespace tests_parser_stmt_program
