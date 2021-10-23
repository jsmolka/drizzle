TEST_CASE("tokenizer_blank_lines_1")
{
    constexpr auto kSource = "noop\n\nnoop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_blank_lines_2")
{
    constexpr auto kSource = "noop\n  \nnoop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_blank_lines_3")
{
    constexpr auto kSource = "noop\n\t\nnoop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_blank_lines_4")
{
    constexpr auto kSource = "noop\n\r\nnoop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_blank_lines_5")
{
    constexpr auto kSource = "noop\n# Comment\nnoop\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_blank_lines_6")
{
    constexpr auto kSource = "\n\nnoop\n\n";

    tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("tokenizer_blank_lines_7")
{
    constexpr auto kSource = "\n\n";

    tokenize(kSource, {
        Token::Type::Eof
    });
}
