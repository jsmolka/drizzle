TEST_CASE("scanner_comments_1")
{
    constexpr auto kSource = "# Comment";

    scan(kSource, {
        Token::Type::Eof
    });
}

TEST_CASE("scanner_comments_2")
{
    constexpr auto kSource = "|# Comment\n";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_comments_3")
{
    constexpr auto kSource = R"(
# Comment
|  # Comment
|
)";

    scan(kSource, {
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Pipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}
