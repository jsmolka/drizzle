TEST_CASE("tokenizer_whitespace_1")
{
    constexpr auto kSource = "noop noop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_whitespace_2")
{
    constexpr auto kSource = "noop\tnoop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_whitespace_3")
{
    constexpr auto kSource = "noop\rnoop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}
