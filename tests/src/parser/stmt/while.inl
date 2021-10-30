TEST_CASE("parser_stmt_while_1")
{
    constexpr auto kSource = R"(
while true:
  noop
)";

    parse(kSource, R"(program
  while
    literal true
    noop
)");
}

TEST_CASE("parser_stmt_while_2")
{
    constexpr auto kSource = R"(
while 1 == 1:
  noop
)";

    parse(kSource, R"(program
  while
    binary ==
      literal 1
      literal 1
    noop
)");
}

TEST_CASE("parser_stmt_while_3")
{
    constexpr auto kSource = R"(
while:
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_while_4")
{
    constexpr auto kSource = R"(
while true
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_while_5")
{
    constexpr auto kSource = R"(
while true: noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_while_6")
{
    constexpr auto kSource = R"(
while true:
)";

    parseThrows(kSource);
}
