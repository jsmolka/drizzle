TEST_CASE("tokenizer_eof_1")
{
    constexpr auto kSource = "";

    tokenize(kSource, {
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_eof_2")
{
    constexpr auto kSource = " ";

    tokenize(kSource, {
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_eof_3")
{
    constexpr auto kSource = "  ";

    tokenize(kSource, {
        Token::Type::Eof
    });
}
