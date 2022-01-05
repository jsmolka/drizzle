#pragma once

#include "utils.h"

namespace tests_parser_expr_precedence {

inline suite _ = [] {
  "parser_expr_precedence"_test = [] {
    {
      for (int i = 0; i < int(Expression::Unary::Type::LastEnumValue); ++i) {
        const auto type = Expression::Unary::Type(i);
        {
          const auto source = fmt::format("{}1 * 2\n", type);
          const auto expect = fmt::format(R"(program
  expression_statement
    binary *
      unary {}
        literal 1
      literal 2)", type);
          parse(source, expect);
        }
        {
          const auto source = fmt::format("{}(1 * 2)\n", type);
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
        const auto& uppers = precedences[i];
        const auto& lowers = precedences[i + 1];

        for (const auto& upper : uppers) {
          for (const auto& lower : lowers) {
            {
              const auto source = fmt::format("1 {} 2 {} 3\n", lower, upper);
              const auto expect = fmt::format(R"(program
  expression_statement
    binary {}
      literal 1
      binary {}
        literal 2
        literal 3)", lower, upper);
              parse(source, expect);
            }
            {
              const auto source = fmt::format("(1 {} 2) {} 3\n", lower, upper);
              const auto expect = fmt::format(R"(program
  expression_statement
    binary {}
      binary {}
        literal 1
        literal 2
      literal 3)", upper, lower);
              parse(source, expect);
            }
          }
        }
      }
    }
  };
};

}  // namespace tests_parser_expr_precedence
