TEST_CASE("tokenizer_comments_1")
{
    constexpr auto kSource = "# Comment";

    tokenize(kSource, {
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_comments_2")
{
    constexpr auto kSource = "noop# Comment\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
        });
}

TEST_CASE("tokenizer_comments_3")
{
    constexpr auto kSource = R"(
# Comment
noop  # Comment
noop
)";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}
