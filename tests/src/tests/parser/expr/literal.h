#pragma once

#include "utils.h"

namespace tests_parser_expr_literal {

inline suite _ = [] {
  "parser_expr_literal"_test = [] {
    static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);

    for (const auto& literal : { "true", "false", "1.1", "1", "null", R"("string")" }) {
      const auto source = fmt::format(R"({})", literal);
      const auto expect = fmt::format(R"(program
  expression_statement
    literal {})", literal);
      parse(source, expect);
    }
  };
};

}  // namespace tests_parser_expr_literal
