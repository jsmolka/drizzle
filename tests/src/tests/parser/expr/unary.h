#pragma once

#include "utils.h"

namespace tests_parser_expr_unary {

inline suite _ = [] {
  "parser_expr_unary"_test = [] {
    for (int i = 0; i < int(Expression::Unary::Type::LastEnumValue); ++i) {
      const auto unary  = Expression::Unary::Type(i);
      const auto source = fmt::format(R"({}1)", unary);
      const auto expect = fmt::format(R"(program
  expression_statement
    unary {}
      literal 1)", unary);
      parse(source, expect);
    }
  };
};

}  // tests_parser_expr_unary
