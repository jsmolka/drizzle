TEST_CASE("scanner_blank_lines_1")
{
    constexpr auto kSource = "|\n\n|\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_2")
{
    constexpr auto kSource = "|\n  \n|\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_3")
{
    constexpr auto kSource = "|\n\t\n|\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_4")
{
    constexpr auto kSource = "|\n\r\n|\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_5")
{
    constexpr auto kSource = "|\n# Comment\n|\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_6")
{
    constexpr auto kSource = "\n\n|\n\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_blank_lines_7")
{
    constexpr auto kSource = "\n\n";

    scan(kSource, {
        Token::Type::Eof
    });
}
