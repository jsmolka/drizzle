TEST_CASE("parser_expr_variable_1")
{
    constexpr auto kSource = "x = 1\n";

    parse(kSource, R"(program
  expression_statement
    assign x
      literal 1
)");
}

TEST_CASE("parser_expr_variable_2")
{
    constexpr auto kSource = "x = 1 == 2\n";

    parse(kSource, R"(program
  expression_statement
    assign x
      binary ==
        literal 1
        literal 2
)");
}

TEST_CASE("parser_expr_variable_3")
{
    constexpr auto kSource = "x = y == z\n";

    parse(kSource, R"(program
  expression_statement
    assign x
      binary ==
        variable y
        variable z
)");
}
