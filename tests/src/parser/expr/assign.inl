TEST_CASE("parser_expr_assign_1")
{
    constexpr auto kSource = "x\n";

    parse(kSource, R"(program
  expression_statement
    variable x
)");
}

TEST_CASE("parser_expr_assign_2")
{
    constexpr auto kSource = "x == y\n";

    parse(kSource, R"(program
  expression_statement
    binary ==
      variable x
      variable y
)");
}

TEST_CASE("parser_expr_assign_3")
{
    constexpr auto kSource = "var x = y = 1\n";

    parse(kSource, R"(program
  var x
    assign y
      literal 1
)");
}

TEST_CASE("parser_expr_assign_4")
{
    constexpr auto kSource = "var x = 1 && 1 && y = 1\n";

    parseThrows(kSource);
}
