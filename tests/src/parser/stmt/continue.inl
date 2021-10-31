TEST_CASE("parser_stmt_continue_1")
{
    constexpr auto kSource = R"(
continue
)";

    parse(kSource, R"(program
  continue
)");
}

TEST_CASE("parser_stmt_continue_2")
{
    constexpr auto kSource = R"(
while true:
  continue
)";

    parse(kSource, R"(program
  while
    literal true
    continue
)");
}

TEST_CASE("parser_stmt_continue_3")
{
    constexpr auto kSource = "continue";

    parseThrows(kSource);
}
