TEST_CASE("parser_stmt_program")
{
    constexpr auto kSource = "null\nnull\nnull\n";

    parse(kSource, R"(program
  expression_statement
    literal null
  expression_statement
    literal null
  expression_statement
    literal null
)");
}
