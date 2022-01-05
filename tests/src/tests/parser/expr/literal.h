#pragma once

#include "utils.h"

namespace tests_parser_expr_literal {

inline suite _ = [] {
  "parser_expr_literal"_test = [] {
    static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);
    static constexpr std::string_view kLiterals[] = {
      "true", "false", "1.1", "1", "null", "\"string\"",
    };

    for (const auto& literal : kLiterals) {
      const auto source = fmt::format("{}\n", literal);
      const auto expect = fmt::format(R"(program
  expression_statement
    literal {})", literal);

      parse(source, expect);
    }
  };
};

}  // namespace tests_parser_expr_literal
