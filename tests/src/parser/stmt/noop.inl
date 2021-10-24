TEST_CASE("parser_stmt_noop_1")
{
    constexpr auto kSource = "noop\n";

    parse(kSource, R"(program
  noop
)");
}

TEST_CASE("parser_stmt_noop_2")
{
    constexpr auto kSource = "noop";

    parseThrows(kSource);
}
