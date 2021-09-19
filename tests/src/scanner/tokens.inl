TEST_CASE("scanner_tokens_1")
{
    constexpr auto kSource = "& && ! != { } [ ] ^ : , . = == > >> >>> >= < << <= - ( ) % | || + / // * ~\n";

    scan(kSource, { 
        Token::Type::And,
        Token::Type::And2,
        Token::Type::Bang,
        Token::Type::BangEqual,
        Token::Type::BraceLeft,
        Token::Type::BraceRight,
        Token::Type::BracketLeft,
        Token::Type::BracketRight,
        Token::Type::Caret,
        Token::Type::Colon,
        Token::Type::Comma,
        Token::Type::Dot,
        Token::Type::Equal,
        Token::Type::Equal2,
        Token::Type::Greater,
        Token::Type::Greater2,
        Token::Type::Greater3,
        Token::Type::GreaterEqual,
        Token::Type::Less,
        Token::Type::Less2,
        Token::Type::LessEqual,
        Token::Type::Minus,
        Token::Type::ParenLeft,
        Token::Type::ParenRight,
        Token::Type::Percent,
        Token::Type::Pipe,
        Token::Type::Pipe2,
        Token::Type::Plus,
        Token::Type::Slash,
        Token::Type::Slash2,
        Token::Type::Star,
        Token::Type::Tilde,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_tokens_2")
{
    constexpr auto kSource = "&&&\n";

    scan(kSource, {
        Token::Type::And2,
        Token::Type::And,
        Token::Type::NewLine,
        Token::Type::Eof
    });
}

TEST_CASE("scanner_tokens_3")
{
    constexpr auto kSource = "var x = $\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_4")
{
    constexpr auto kSource = "var x = \\\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_5")
{
    constexpr auto kSource = "var x = ``\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_6")
{
    constexpr auto kSource = "var x = ?\n";

    scanThrows(kSource);
}

TEST_CASE("scanner_tokens_7")
{
    constexpr auto kSource = "var x = ä\n";

    scanThrows(kSource);
}
