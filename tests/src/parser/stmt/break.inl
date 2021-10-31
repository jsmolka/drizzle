TEST_CASE("parser_stmt_break_1")
{
    constexpr auto kSource = R"(
break
)";

    parse(kSource, R"(program
  break
)");
}

TEST_CASE("parser_stmt_break_2")
{
    constexpr auto kSource = R"(
while true:
  break
)";

    parse(kSource, R"(program
  while
    literal true
    break
)");
}

TEST_CASE("parser_stmt_break_3")
{
    constexpr auto kSource = "break";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_break_4")
{
    constexpr auto kSource = R"(
break label
)";

    parse(kSource, R"(program
  break label
)");
}

TEST_CASE("parser_stmt_break_5")
{
    constexpr auto kSource = R"(
block:
  break label
)";

    parse(kSource, R"(program
  block
    break label
)");
}

TEST_CASE("parser_stmt_break_6")
{
    constexpr auto kSource = "break label";

    parseThrows(kSource);
}
