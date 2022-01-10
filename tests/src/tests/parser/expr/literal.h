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

  "parser_expr_literal_string"_test = [] {
    {
      constexpr std::tuple<const char*, const char*> kCases[] = {
        {R"("""string""")", "string"},
        {R"("""string
""")", "string\n"},
        {R"("""string\""")", "string\\"},
        {R"("""string """)", "string "},
        {R"("string")", "string"},
        {R"("\\\"\n\r\t")", "\\\"\n\r\t"}
      };

      for (const auto& [source, string] : kCases) {
        const auto expect = fmt::format(R"(program
  expression_statement
    literal "{}")", string);
        parse(source, expect);
      }
    }
    {
      for (int c = 0; c < 256; ++c) {
        switch (c) {
          case '\\':
          case '"':
          case 'n':
          case 'r':
          case 't':
            continue;
        }
        parseThrows(fmt::format(R"("\{}")", c));
      }
    }
  };

  "parser_expr_literal_integer"_test = [] {
    constexpr std::tuple<const char*, dzint> kCases[] = {
      {"0b0", 0b0},
      {"0b01001", 0b01001},
      {"0x0", 0x0},
      {"0xabcdef", 0xabcdef},
      {"0xABCDEF", 0xABCDEF},
      {"100", 100}
    };

    for (const auto& [source, number] : kCases) {
      const auto expect = fmt::format(R"(program
  expression_statement
    literal {})", number);
      parse(source, expect);
    }
  };

  "parser_expr_literal_float"_test = [] {
    constexpr std::tuple<const char*, dzfloat> kCases[] = {
      {"1.1", 1.1}
    };

    for (const auto& [source, number] : kCases) {
      const auto expect = fmt::format(R"(program
  expression_statement
    literal {})", number);
      parse(source, expect);
    }
  };
};

}  // namespace tests_parser_expr_literal
