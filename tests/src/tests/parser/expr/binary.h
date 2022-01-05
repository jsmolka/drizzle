#pragma once

#include "utils.h"

namespace tests_parser_expr_binary {

inline suite _ = [] {
  "parser_expr_binary"_test = [] {
    for (int i = 0; i < int(Expression::Binary::Type::LastEnumValue); ++i) {
      const auto type = Expression::Binary::Type(i);
      const auto source = fmt::format("1 {} 2\n", type);
      const auto expect = fmt::format(R"(program
  expression_statement
    binary {}
      literal 1
      literal 2)", type);

      parse(source, expect);
    }
  };
};

}  // namespace tests_parser_expr_binary
