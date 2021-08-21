TEST_CASE("scanner_whitespace_1")
{
    using Type = Token::Type;

    constexpr auto kSource = "| |\n";

    scan(kSource, {
        Type::Pipe,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_whitespace_2")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\t|\n";

    scan(kSource, {
        Type::Pipe,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_whitespace_3")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\r|\n";

    scan(kSource, {
        Type::Pipe,
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}
