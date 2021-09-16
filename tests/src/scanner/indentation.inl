TEST_CASE("scanner_indentation_1")
{
    constexpr auto kSource = R"(
| # 1
  | # 2
    | # 3
  | # 4
| # 5
)";

    scan(kSource, { 
        Token::Type::Pipe,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Pipe,  // 4
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Pipe,  // 5
        Token::Type::NewLine,
        Token::Type::Eof 
    });
}

TEST_CASE("scanner_indentation_2")
{
    constexpr auto kSource = R"(
| # 1
  | # 2
  | # 3
| # 4
)";

    scan(kSource, { 
        Token::Type::Pipe,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,  // 2
        Token::Type::NewLine,
        Token::Type::Pipe,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Pipe,  // 4
        Token::Type::NewLine,
        Token::Type::Eof 
    });
}

TEST_CASE("scanner_indentation_3")
{
    constexpr auto kSource = R"(
| # 1
  | # 2
    | # 3
)";

    scan(kSource, { 
        Token::Type::Pipe,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Dedent,
        Token::Type::Eof 
    });
}

TEST_CASE("scanner_indentation_4")
{
    constexpr auto kSource = R"(
| # 1
  | # 2
    | # 3

    | # 4
)";

    scan(kSource, { 
        Token::Type::Pipe,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,  // 3
        Token::Type::NewLine,
        Token::Type::Pipe,  // 4
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Dedent,
        Token::Type::Eof 
    });
}

TEST_CASE("scanner_indentation_5")
{
    constexpr auto kSource = "\r  |\n";

    scan(kSource, { 
        Token::Type::Indent,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Eof 
    });
}

TEST_CASE("scanner_indentation_6")
{
    constexpr auto kSource = " |";

    scanThrows(kSource);
}

TEST_CASE("scanner_indentation_7")
{
    constexpr auto kSource = "\t|";

    scanThrows(kSource);
}

TEST_CASE("scanner_indentation_8")
{
    constexpr auto kSource = "    |";

    scanThrows(kSource);
}

TEST_CASE("scanner_indentation_9")
{
    constexpr auto kSource = R"(
|
  |
)";

    scan(kSource, { 
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Eof 
    });
}
