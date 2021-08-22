TEST_CASE("scanner_whitespace_1")
{
    constexpr auto kSource = "| |\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_whitespace_2")
{
    constexpr auto kSource = "|\t|\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_whitespace_3")
{
    constexpr auto kSource = "|\r|\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}
