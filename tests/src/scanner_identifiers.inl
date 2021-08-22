TEST_CASE("scanner_identifiers_1")
{
    constexpr auto kSource = "| break |";

    scanLexeme(kSource, Token::Type::Break, "break");
}

TEST_CASE("scanner_identifiers_2")
{
    constexpr auto kSource = "| class |";

    scanLexeme(kSource, Token::Type::Class, "class");
}

TEST_CASE("scanner_identifiers_3")
{
    constexpr auto kSource = "| continue |";

    scanLexeme(kSource, Token::Type::Continue, "continue");
}

TEST_CASE("scanner_identifiers_4")
{
    constexpr auto kSource = "| elif |";

    scanLexeme(kSource, Token::Type::Elif, "elif");
}

TEST_CASE("scanner_identifiers_5")
{
    constexpr auto kSource = "| else |";

    scanLexeme(kSource, Token::Type::Else, "else");
}

TEST_CASE("scanner_identifiers_6")
{
    constexpr auto kSource = "| extends |";

    scanLexeme(kSource, Token::Type::Extends, "extends");
}

TEST_CASE("scanner_identifiers_7")
{
    constexpr auto kSource = "| false |";

    scanLexeme(kSource, Token::Type::False, "false");
}

TEST_CASE("scanner_identifiers_8")
{
    constexpr auto kSource = "| for |";

    scanLexeme(kSource, Token::Type::For, "for");
}

TEST_CASE("scanner_identifiers_9")
{
    constexpr auto kSource = "| function |";

    scanLexeme(kSource, Token::Type::Function, "function");
}

TEST_CASE("scanner_identifiers_10")
{
    constexpr auto kSource = "| if |";

    scanLexeme(kSource, Token::Type::If, "if");
}

TEST_CASE("scanner_identifiers_11")
{
    constexpr auto kSource = "| in |";

    scanLexeme(kSource, Token::Type::In, "in");
}

TEST_CASE("scanner_identifiers_12")
{
    constexpr auto kSource = "| iterator |";

    scanLexeme(kSource, Token::Type::Iterator, "iterator");
}

TEST_CASE("scanner_identifiers_13")
{
    constexpr auto kSource = "| noop |";

    scanLexeme(kSource, Token::Type::Noop, "noop");
}

TEST_CASE("scanner_identifiers_14")
{
    constexpr auto kSource = "| null |";

    scanLexeme(kSource, Token::Type::Null, "null");
}

TEST_CASE("scanner_identifiers_15")
{
    constexpr auto kSource = "| return |";

    scanLexeme(kSource, Token::Type::Return, "return");
}

TEST_CASE("scanner_identifiers_16")
{
    constexpr auto kSource = "| super |";

    scanLexeme(kSource, Token::Type::Super, "super");
}

TEST_CASE("scanner_identifiers_17")
{
    constexpr auto kSource = "| this |";

    scanLexeme(kSource, Token::Type::This, "this");
}

TEST_CASE("scanner_identifiers_18")
{
    constexpr auto kSource = "| true |";

    scanLexeme(kSource, Token::Type::True, "true");
}

TEST_CASE("scanner_identifiers_19")
{
    constexpr auto kSource = "| var |";

    scanLexeme(kSource, Token::Type::Var, "var");
}

TEST_CASE("scanner_identifiers_20")
{
    constexpr auto kSource = "| while |";

    scanLexeme(kSource, Token::Type::While, "while");
}

TEST_CASE("scanner_identifiers_21")
{
    constexpr auto kSource = "| yield |";

    scanLexeme(kSource, Token::Type::Yield, "yield");
}

TEST_CASE("scanner_identifiers_22")
{
    using Type = Token::Type;

    constexpr auto kSource = "yield|yielding\n";

    scan(kSource, {
        Type::Yield,
        Type::Pipe,
        Type::Identifier,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_identifiers_23")
{
    using Type = Token::Type;

    constexpr auto kSource = "0xAyield\n";

    scan(kSource, {
        Type::Integer,
        Type::Yield,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_identifiers_24")
{
    using Type = Token::Type;

    constexpr auto kSource = "break breaking Breaking\n";

    scan(kSource, {
        Type::Break,
        Type::Identifier,
        Type::Identifier,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_identifiers_25")
{
    using Type = Token::Type;

    constexpr auto kSource = "var x = \"test\"\n";

    scan(kSource, {
        Type::Var,
        Type::Identifier,
        Type::Equal,
        Type::String,
        Type::NewLine,
        Type::Eof
    });
}

TEST_CASE("scanner_identifiers_26")
{
    using Type = Token::Type;

    constexpr auto kSource = "var x=\"test\"\n";

    scan(kSource, {
        Type::Var,
        Type::Identifier,
        Type::Equal,
        Type::String,
        Type::NewLine,
        Type::Eof
    });
}
