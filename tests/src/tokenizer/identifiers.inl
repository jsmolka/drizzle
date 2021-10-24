TEST_CASE("tokenizer_identifiers_1")
{
    constexpr auto kSource = "break breaking\n";

    tokenize(kSource, {
        Token::Type::Break,
        Token::Type::Identifier,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_identifiers_2")
{
    constexpr auto kSource = "continue continued\n";

    tokenize(kSource, {
        Token::Type::Continue,
        Token::Type::Identifier,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_identifiers_3")
{
    constexpr auto kSource = "0noop\n";

    tokenize(kSource, {
        Token::Type::Integer,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_identifiers_4")
{
    constexpr auto kSource = "0identifier\n";

    tokenize(kSource, {
        Token::Type::Integer,
        Token::Type::Identifier,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}
