#pragma once

#include "utils.h"

namespace tests_parser_stmt_var {

inline suite _ = [] {
  "parser_stmt_var"_test = [] {
    {
      constexpr auto kSource = "var x = 0\n";
      parse(kSource, R"(program
  var x
    literal 0)");
    }
    {
      constexpr auto kSource = "var x\n";
      parse(kSource, R"(program
  var x
    literal null)");
    }
    {
      constexpr auto kSource = "var\n";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = "var x =\n";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = "var x =";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_stmt_var
