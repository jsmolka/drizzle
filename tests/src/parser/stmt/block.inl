TEST_CASE("parser_stmt_block_1")
{
    constexpr auto kSource = R"(
block:
  noop
  noop
)";

    parse(kSource, R"(program
  block
    noop
    noop
)");
}

TEST_CASE("parser_stmt_block_2")
{
    constexpr auto kSource = R"(
block name:
  noop
  noop
)";

    parse(kSource, R"(program
  block name
    noop
    noop
)");
}

TEST_CASE("parser_stmt_block_3")
{
    constexpr auto kSource = R"(
block name
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_block_4")
{
    constexpr auto kSource = R"(
block
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_block_5")
{
    constexpr auto kSource = R"(
block: noop
)";

    parseThrows(kSource);
}
