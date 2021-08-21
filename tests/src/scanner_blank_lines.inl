TEST_CASE("scanner_blank_lines_1")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\n\n|\n";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_2")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\n  \n|\n";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_3")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\n\t\n|\n";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_4")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\n\r\n|\n";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_5")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\n# Comment\n|\n";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_6")
{
    using Type = Token::Type;

    constexpr auto kSource = "\n\n|\n\n";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_7")
{
    using Type = Token::Type;

    constexpr auto kSource = "\n\n";

    scan(kSource, {
        Type::Eof
    });
}
