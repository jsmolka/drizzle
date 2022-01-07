#pragma once

#include "utils.h"

namespace tests_parser_expr_precedence {

inline suite _ = [] {
  "parser_expr_precedence"_test = [] {
    {
      for (int i = 0; i < int(Expression::Unary::Type::LastEnumValue); ++i) {
        const auto type = Expression::Unary::Type(i);
        {
          const auto source = fmt::format(R"({}1 * 2)", type);
          const auto expect = fmt::format(R"(program
  expression_statement
    binary *
      unary {}
        literal 1
      literal 2)", type);
          parse(source, expect);
        }
        {
          const auto source = fmt::format(R"({}(1 * 2))", type);
          const auto expect = fmt::format(R"(program
  expression_statement
    unary {}
      binary *
        literal 1
        literal 2)", type);
          parse(source, expect);
        }
      }
    }
    {
      static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

      std::vector<std::vector<std::string_view>> precedences = {
        {"*", "/", "%", "//", "**"},
        {"+", "-"},
        {"<<", ">>", ">>>"},
        {"<", "<=", ">", ">="},
        {"==", "!="},
        {"&"},
        {"^"},
        {"|"},
        {"&&"},
        {"||"},
      };

      for (int i = 0; i < precedences.size() - 1; ++i) {
        const auto& highs = precedences[i];
        const auto& lows  = precedences[i + 1];

        for (const auto& high : highs) {
          for (const auto& low : lows) {
            {
              const auto source = fmt::format(R"(1 {} 2 {} 3)", low, high);
              const auto expect = fmt::format(R"(program
  expression_statement
    binary {}
      literal 1
      binary {}
        literal 2
        literal 3)", low, high);
              parse(source, expect);
            }
            {
              const auto source = fmt::format(R"((1 {} 2) {} 3)", low, high);
              const auto expect = fmt::format(R"(program
  expression_statement
    binary {}
      binary {}
        literal 1
        literal 2
      literal 3)", high, low);
              parse(source, expect);
            }
          }
        }
      }
    }
  };
};

}  // namespace tests_parser_expr_precedence
