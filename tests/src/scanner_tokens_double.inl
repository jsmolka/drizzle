TEST_CASE("scanner_tokens_double")
{
    using Type = Token::Type;

    constexpr auto kSource = "& && ! != = == > >= < <= | ||";

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
        Type::Eof
    });
}

TEST_CASE("scanner_tokens_double_2")
{
    using Type = Token::Type;

    constexpr auto kSource = "&&&";

    scan(kSource, {
        Type::AndAnd,
        Type::And,
        Type::Eof
    });
}
