TEST_CASE("parser_expr_group_1") {
  constexpr auto kSource = "(1 * 2) + (3 * 4)\n";

  parse(kSource, R"(program
  expression_statement
    binary +
      binary *
        literal 1
        literal 2
      binary *
        literal 3
        literal 4
)");
}

TEST_CASE("parser_expr_group_2") {
  constexpr auto kSource = "1 * (2 + 3) * 4\n";

  parse(kSource, R"(program
  expression_statement
    binary *
      binary *
        literal 1
        binary +
          literal 2
          literal 3
      literal 4
)");
}
