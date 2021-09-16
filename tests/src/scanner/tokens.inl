TEST_CASE("scanner_tokens_1")
{
    constexpr auto kSource = "{}[]^:,.-()%+/*\n";

    scan(kSource, { 
        Token::Type::BraceLeft,
        Token::Type::BraceRight,
        Token::Type::BracketLeft,
        Token::Type::BracketRight,
        Token::Type::Caret,
        Token::Type::Colon,
        Token::Type::Comma,
        Token::Type::Dot,
        Token::Type::Minus,
        Token::Type::ParenLeft,
        Token::Type::ParenRight,
        Token::Type::Percent,
        Token::Type::Plus,
        Token::Type::Slash,
        Token::Type::Star,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_tokens_2")
{
    constexpr auto kSource = "& && ! != = == > >= < <= | ||\n";

    scan(kSource, {
        Token::Type::And,
        Token::Type::AndAnd,
        Token::Type::Bang,
        Token::Type::BangEqual,
        Token::Type::Equal,
        Token::Type::EqualEqual,
        Token::Type::Greater,
        Token::Type::GreaterEqual,
        Token::Type::Less,
        Token::Type::LessEqual,
        Token::Type::Pipe,
        Token::Type::PipePipe,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_tokens_3")
{
    constexpr auto kSource = "&&&\n";

    scan(kSource, {
        Token::Type::AndAnd,
        Token::Type::And,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_tokens_4")
{
    constexpr auto kSource = "var x = $\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_5")
{
    constexpr auto kSource = "var x = \\\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_6")
{
    constexpr auto kSource = "var x = ~\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_7")
{
    constexpr auto kSource = "var x = ``\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_8")
{
    constexpr auto kSource = "var x = ?\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_9")
{
    constexpr auto kSource = "var x = ä\n";

    scanThrows(kSource);
}
