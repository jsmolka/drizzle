TEST_CASE("scanner_whitespace")
{
    using Type = Token::Type;

    constexpr auto kSource = "| |";

    scan(kSource, {
        Type::Pipe,
        Type::Pipe,
        Type::Eof
    });
}

TEST_CASE("scanner_whitespace_2")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\t|";

    scan(kSource, {
        Type::Pipe,
        Type::Pipe,
        Type::Eof
    });
}

TEST_CASE("scanner_whitespace_3")
{
    using Type = Token::Type;

    constexpr auto kSource = "|\r|";

    scan(kSource, {
        Type::Pipe,
        Type::Pipe,
        Type::Eof
    });
}
