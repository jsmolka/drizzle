#pragma once

#include "utils.h"

namespace tests_parser_stmt_print {

inline suite _ = [] {
  "parser_stmt_print"_test = [] {
    {
      constexpr auto kSource = R"(print 1)";
      constexpr auto kExpect = R"(program
  print
    literal 1)";
      parse(kSource, kExpect);
    }
  };
};

}  // namespace tests_parser_stmt_print
