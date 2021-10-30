TEST_CASE("parser_stmt_if_1")
{
    constexpr auto kSource = R"(
if true:
  noop
)";

    parse(kSource, R"(program
  if
    literal true
    noop
)");
}

TEST_CASE("parser_stmt_if_2")
{
    constexpr auto kSource = R"(
if 1 == 1:
  noop
)";

    parse(kSource, R"(program
  if
    binary ==
      literal 1
      literal 1
    noop
)");
}

TEST_CASE("parser_stmt_if_3")
{
    constexpr auto kSource = R"(
if 1 == 1:
  noop
  noop
)";

    parse(kSource, R"(program
  if
    binary ==
      literal 1
      literal 1
    noop
    noop
)");
}

TEST_CASE("parser_stmt_if_4")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
)";

    parse(kSource, R"(program
  if
    literal true
    noop
  elif
    literal true
    noop
)");
}

TEST_CASE("parser_stmt_if_5")
{
    constexpr auto kSource = R"(
if true:
  noop
elif 1 == 1:
  noop
)";

    parse(kSource, R"(program
  if
    literal true
    noop
  elif
    binary ==
      literal 1
      literal 1
    noop
)");
}

TEST_CASE("parser_stmt_if_6")
{
    constexpr auto kSource = R"(
if true:
  noop
elif 1 == 1:
  noop
  noop
)";

    parse(kSource, R"(program
  if
    literal true
    noop
  elif
    binary ==
      literal 1
      literal 1
    noop
    noop
)");
}

TEST_CASE("parser_stmt_if_7")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else:
  noop
)";

    parse(kSource, R"(program
  if
    literal true
    noop
  elif
    literal true
    noop
  else
    noop
)");
}

TEST_CASE("parser_stmt_if_8")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else:
  noop
  noop
)";

    parse(kSource, R"(program
  if
    literal true
    noop
  elif
    literal true
    noop
  else
    noop
    noop
)");
}

TEST_CASE("parser_stmt_if_9")
{
    constexpr auto kSource = R"(
if:
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_10")
{
    constexpr auto kSource = R"(
if true
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_11")
{
    constexpr auto kSource = R"(
if true:
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_12")
{
    constexpr auto kSource = R"(
if true: pass
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_13")
{
    constexpr auto kSource = R"(
if true:
  noop
elif:
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_14")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_15")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true:
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_16")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true: noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_17")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else
  noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_18")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else:
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_19")
{
    constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
elif: noop
)";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_20")
{
    constexpr auto kSource = "elif\n";

    parseThrows(kSource);
}

TEST_CASE("parser_stmt_if_21")
{
    constexpr auto kSource = "else\n";

    parseThrows(kSource);
}
