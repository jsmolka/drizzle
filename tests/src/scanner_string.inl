TEST_CASE("scanner_string")
{
    constexpr auto kSource = "\"test\"";

    scanLexeme(kSource, Token::Type::String, "\"test\"");
}

TEST_CASE("scanner_string_1")
{
    constexpr auto kSource = "|\"test\"|";

    scanLexeme(kSource, Token::Type::String, "\"test\"");
}

TEST_CASE("scanner_string_2")
{
    constexpr auto kSource = R"(
"test

"
)";

    scanLexeme(kSource, Token::Type::String, "\"test\n\n\"");
}

TEST_CASE("scanner_string_3")
{
    constexpr auto kSource = R"(
if x == 0:
  var y = "test

"
)";

    scanLexeme(kSource, Token::Type::String, "\"test\n\n\"");
}

TEST_CASE("scanner_string_4")
{
    constexpr auto kSource = "\"test";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}
