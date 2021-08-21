TEST_CASE("scanner_tokens_1")
{
    using Type = Token::Type;

    constexpr auto kSource = "{}[]^:,.-()%+/*\n";

    scan(kSource, { 
        Type::BraceLeft,
        Type::BraceRight,
        Type::BracketLeft,
        Type::BracketRight,
        Type::Caret,
        Type::Colon,
        Type::Comma,
        Type::Dot,
        Type::Minus,
        Type::ParenLeft,
        Type::ParenRight,
        Type::Percent,
        Type::Plus,
        Type::Slash,
        Type::Star,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_tokens_2")
{
    using Type = Token::Type;

    constexpr auto kSource = "& && ! != = == > >= < <= | ||\n";

    scan(kSource, {
        Type::And,
        Type::AndAnd,
        Type::Bang,
        Type::BangEqual,
        Type::Equal,
        Type::EqualEqual,
        Type::Greater,
        Type::GreaterEqual,
        Type::Less,
        Type::LessEqual,
        Type::Pipe,
        Type::PipePipe,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_tokens_3")
{
    using Type = Token::Type;

    constexpr auto kSource = "&&&\n";

    scan(kSource, {
        Type::AndAnd,
        Type::And,
        Type::NewLine,
        Type::Eof
    });
}
