TEST_CASE("scanner_eof_1")
{
    constexpr auto kSource = "";

    scan(kSource, {
        Token::Type::Eof
    });
}

TEST_CASE("scanner_eof_2")
{
    constexpr auto kSource = " ";

    scan(kSource, {
        Token::Type::Eof
    });
}

TEST_CASE("scanner_eof_3")
{
    constexpr auto kSource = "  ";

    scan(kSource, {
        Token::Type::Eof
    });
}
