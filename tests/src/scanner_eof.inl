TEST_CASE("scanner_eof")
{
    using Type = Token::Type;

    constexpr auto kSource = "";

    scan(kSource, {
        Type::Eof
    });
}

TEST_CASE("scanner_eof_2")
{
    using Type = Token::Type;

    constexpr auto kSource = " ";

    scan(kSource, {
        Type::Eof
    });
}

TEST_CASE("scanner_eof_3")
{
    using Type = Token::Type;

    constexpr auto kSource = "  ";

    scan(kSource, {
        Type::Eof
    });
}
