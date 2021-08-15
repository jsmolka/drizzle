TEST_CASE("scanner_indentation")
{
    using Type = Token::Type;

    constexpr auto kSource = R"(
| # 1
  | # 2
    | # 3
  | # 4
| # 5)";

    scan(kSource, { 
        Type::Pipe,  // 1
        Type::NewLine,
        Type::Indent,
        Type::Pipe,  // 2
        Type::NewLine,
        Type::Indent,
        Type::Pipe,  // 3
        Type::NewLine,
        Type::Dedent,
        Type::Pipe,  // 4
        Type::NewLine,
        Type::Dedent,
        Type::Pipe,  // 5
        Type::Eof 
    });
}

TEST_CASE("scanner_indentation_2")
{
    using Type = Token::Type;

    constexpr auto kSource = R"(
| # 1
  | # 2
  | # 3
| # 4)";

    scan(kSource, { 
        Type::Pipe,  // 1
        Type::NewLine,
        Type::Indent,
        Type::Pipe,  // 2
        Type::NewLine,
        Type::Pipe,  // 3
        Type::NewLine,
        Type::Dedent,
        Type::Pipe,  // 4
        Type::Eof 
    });
}

TEST_CASE("scanner_indentation_3")
{
    using Type = Token::Type;

    constexpr auto kSource = R"(
| # 1
  | # 2
    | # 3
)";

    scan(kSource, { 
        Type::Pipe,  // 1
        Type::NewLine,
        Type::Indent,
        Type::Pipe,  // 2
        Type::NewLine,
        Type::Indent,
        Type::Pipe,  // 3
        Type::NewLine,
        Type::Dedent,
        Type::Dedent,
        Type::Eof 
    });
}

TEST_CASE("scanner_indentation_4")
{
    using Type = Token::Type;

    constexpr auto kSource = R"(
| # 1
  | # 2
    | # 3

    | # 4
)";

    scan(kSource, { 
        Type::Pipe,  // 1
        Type::NewLine,
        Type::Indent,
        Type::Pipe,  // 2
        Type::NewLine,
        Type::Indent,
        Type::Pipe,  // 3
        Type::NewLine,
        Type::Pipe,  // 4
        Type::NewLine,
        Type::Dedent,
        Type::Dedent,
        Type::Eof 
    });
}

TEST_CASE("scanner_indentation_5")
{
    using Type = Token::Type;

    constexpr auto kSource = "\r  |";

    scan(kSource, { 
        Type::Indent,
        Type::Pipe,
        Type::NewLine,
        Type::Dedent,
        Type::Eof 
    });
}

TEST_CASE("scanner_indentation_6")
{
    using Type = Token::Type;

    constexpr auto kSource = " |";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_indentation_7")
{
    using Type = Token::Type;

    constexpr auto kSource = "\t|";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_indentation_8")
{
    using Type = Token::Type;

    constexpr auto kSource = "    |";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_indentation_9")
{
    using Type = Token::Type;

    constexpr auto kSource = R"(
|
  |)";

    scan(kSource, { 
        Type::Pipe,
        Type::NewLine,
        Type::Indent,
        Type::Pipe,
        Type::NewLine,
        Type::Dedent,
        Type::Eof 
    });
}
