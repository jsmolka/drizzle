TEST_CASE("parser_stmt_expression_statement")
{
    constexpr auto kSource = "null\n";

    parse(kSource, R"(program
  expression_statement
    literal null
)");
}
