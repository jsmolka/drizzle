TEST_CASE("scanner_comments")
{
    using Type = Token::Type;

    constexpr auto kSource = "# Comment";

    scan(kSource, { Type::Eof });
}

TEST_CASE("scanner_comments_2")
{
    using Type = Token::Type;

    constexpr auto kSource = "|# Comment";

    scan(kSource, {
        Type::Pipe,
        Type::Eof
    });
}

TEST_CASE("scanner_comments_3")
{
    using Type = Token::Type;

    constexpr auto kSource = "|# Comment\n";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_comments_4")
{
    using Type = Token::Type;

    constexpr auto kSource = R"(
# Comment
|  # Comment
|)";

    scan(kSource, {
        Type::Pipe,
        Type::NewLine,
        Type::Pipe,
        Type::Eof
    });
}
