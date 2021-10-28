TEST_CASE("parser_stmt_variable_definition_1")
{
    constexpr auto kSource = "var x = 0\n";

    parse(kSource, R"(program
  variable_definition x
    literal 0
)");
}

TEST_CASE("parser_stmt_variable_definition_2")
{
    constexpr auto kSource = "var x\n";

    parse(kSource, R"(program
  variable_definition x
    literal null
)");
}

TEST_CASE("parser_stmt_variable_definition_3")
{
    constexpr auto kSource = "var\n";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_variable_definition_4")
{
    constexpr auto kSource = "var x =\n";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_variable_definition_5")
{
    constexpr auto kSource = "var x =";

    parseThrows(kSource);
}
