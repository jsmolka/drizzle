#pragma once

#include "utils.h"

namespace tests_parser_expr_literal {

inline suite _ = [] {
  "parser_expr_literal"_test = [] {
    static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);

    for (const auto& literal : {"true", "false", "1.1", "1", "null", R"("string")"}) {
      const auto source = fmt::format(R"({})", literal);
      const auto expect = fmt::format(R"(program
  expression_statement
    literal {})", literal);
      parse(source, expect);
    }
  };

  "parser_expr_literal_string"_test = [] {
    {
      constexpr std::tuple<const char*, const char*> kTests[] = {
        {R"("string")", "string"},
        {R"("""string""")", "string"},
        {R"("""string\""")", "string\\"},
        {R"("\\\"\n\r\t")", "\\\"\n\r\t"},
      };

      for (const auto& [source, string] : kTests) {
        const auto expect = fmt::format(R"(program
  expression_statement
    literal "{}")", string);
        parse(source, expect);
      }
    }
  };

  "parser_expr_literal_integer"_test = [] {
    constexpr std::tuple<const char*, dzint> kTests[] = {
      {"0b0", 0b0},
      {"0b01001", 0b01001},
      {"0x0", 0x0},
      {"0xabcdef", 0xabcdef},
      {"0xABCDEF", 0xABCDEF},
      {"0", 0},
      {"100", 100},
      {"101", 101},
    };

    for (const auto& [source, number] : kTests) {
      const auto expect = fmt::format(R"(program
  expression_statement
    literal {})", number);
      parse(source, expect);
    }
  };

  "parser_expr_literal_float"_test = [] {
    constexpr std::tuple<const char*, dzfloat> kTests[] = {
      {"0.0", 0.0},
      {"0.1", 0.1},
      {"0.12", 0.12},
      {"0.123", 0.123},
      {"0.1234", 0.1234},
      {"12345.65789", 12345.65789},
    };

    for (const auto& [source, number] : kTests) {
      const auto expect = fmt::format(R"(program
  expression_statement
    literal {})", number);
      parse(source, expect);
    }
  };

  "parse_expr_literal_errors"_test = [] {
    constexpr const char* kSources[] = {
      "0b11111111111111111111111111111111111111111111111111111111111111111",
      "0b111111111111111111111111111111111111111111111111111111111111111111",
      "0b1111111111111111111111111111111111111111111111111111111111111111111",
      "0b11111111111111111111111111111111111111111111111111111111111111111111",
      "0x11111111111111111",
      "0x111111111111111111",
      "0x1111111111111111111",
      "0x11111111111111111111",
      "18446744073709551616",
      "18446744073709551617",
      "18446744073709551618",
      "18446744073709551619",
    };

    for (const auto& source : kSources) {
      parseThrows(source);
    }
  };
};

}  // namespace tests_parser_expr_literal
