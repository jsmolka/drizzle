TEST_CASE("scanner_tokens_single")
{
    using Type = Token::Type;

    constexpr auto kSource = "{}[]^:,.-()%+/*";

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
        Type::Eof
    });
}
