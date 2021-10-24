TEST_CASE("parser_stmt_expression_statement_1")
{
    constexpr auto kSource = "null\n";

    parse(kSource, R"(program
  expression_statement
    literal null
)");
}

TEST_CASE("parser_stmt_expression_statement_2")
{
    constexpr auto kSource = "null";

    parseThrows(kSource);
}
