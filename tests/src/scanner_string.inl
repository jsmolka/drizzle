void scanString(const std::string& source, const std::string& expect)
{
    Scanner scanner;
    auto tokens = scanner.scan(source);
    for (const auto& token : tokens)
    {
        if (token.type == Token::Type::String)
        {
            REQUIRE(token.lexeme == expect);
            return;
        }
    }
    REQUIRE(false);
}

TEST_CASE("scanner_string")
{
    constexpr auto kSource = "\"test\"";

    scanString(kSource, "\"test\"");
}

TEST_CASE("scanner_string_1")
{
    constexpr auto kSource = "|\"test\"|";

    scanString(kSource, "\"test\"");
}

TEST_CASE("scanner_string_2")
{
    constexpr auto kSource = R"(
"test

"
)";

    scanString(kSource, "\"test\n\n\"");
}

TEST_CASE("scanner_string_3")
{
    constexpr auto kSource = R"(
if x == 0:
  var y = "test

"
)";

    scanString(kSource, "\"test\n\n\"");
}

TEST_CASE("scanner_string_4")
{
    constexpr auto kSource = "\"test";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}
