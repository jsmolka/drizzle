#pragma once

#include "utils.h"

namespace tests_parser_stmt_print {

inline suite _ = [] {
  "parser_stmt_print"_test = [] {
    {
      constexpr auto kSource = "print 1\n";
      parse(kSource, R"(program
  print
    literal 1)");
    }
    {
      constexpr auto kSource = "print 1";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_stmt_print
