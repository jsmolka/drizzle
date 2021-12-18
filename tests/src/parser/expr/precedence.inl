TEST_CASE("parser_expr_precedence_1") {
  for (int i = 0; i < int(Expression::Unary::Type::LastEnumValue); ++i) {
    const auto type = Expression::Unary::Type(i);
    auto source = fmt::format("{}1 * 2\n", type);

    SECTION(source) {
      parse(source, fmt::format(R"(program
  expression_statement
    binary *
      unary {}
        literal 1
      literal 2
)",
                                type));
    }

    source = fmt::format("{}(1 * 2)\n", type);

    SECTION(source) {
      parse(source, fmt::format(R"(program
  expression_statement
    unary {}
      binary *
        literal 1
        literal 2
)",
                                type));
    }
  }
}

TEST_CASE("parser_expr_precedence_2") {
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
    const auto& lows = precedences[i + 1];

    for (const auto& high : highs) {
      for (const auto& low : lows) {
        auto source = fmt::format("1 {} 2 {} 3\n", low, high);

        SECTION(source) {
          parse(source, fmt::format(R"(program
  expression_statement
    binary {}
      literal 1
      binary {}
        literal 2
        literal 3
)",
                                    low, high));
        }

        source = fmt::format("(1 {} 2) {} 3\n", low, high);

        SECTION(source) {
          parse(source, fmt::format(R"(program
  expression_statement
    binary {}
      binary {}
        literal 1
        literal 2
      literal 3
)",
                                    high, low));
        }
      }
    }
  }
}
