TEST_CASE("scanner_number")
{
    constexpr auto kSource = "0b0";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_2")
{
    constexpr auto kSource = "0b01001";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_3")
{
    constexpr auto kSource = "var x = 0b01001";

    scanLexeme(kSource, Token::Type::Integer, "0b01001");
}

TEST_CASE("scanner_number_4")
{
    constexpr auto kSource = "var x=0b01001\n";

    scanLexeme(kSource, Token::Type::Integer, "0b01001");
}

TEST_CASE("scanner_number_5")
{
    constexpr auto kSource = "0b";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_6")
{
    constexpr auto kSource = "0b2";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}
TEST_CASE("scanner_number_7")
{
    constexpr auto kSource = "0b_";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_8")
{
    constexpr auto kSource = "0x0";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_9")
{
    constexpr auto kSource = "0xabcdef1234567890";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_10")
{
    constexpr auto kSource = "0xABDCDEF";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_11")
{
    constexpr auto kSource = "var x = 0xDEAD";

    scanLexeme(kSource, Token::Type::Integer, "0xDEAD");
}

TEST_CASE("scanner_number_12")
{
    constexpr auto kSource = "var x=0xDEAD\n";

    scanLexeme(kSource, Token::Type::Integer, "0xDEAD");
}

TEST_CASE("scanner_number_13")
{
    constexpr auto kSource = "0x";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_14")
{
    constexpr auto kSource = "0xn";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_15")
{
    constexpr auto kSource = "0x_";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_16")
{
    constexpr auto kSource = "0b0000000000000000000000000000000000000000000000000000000000000000";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_17")
{
    constexpr auto kSource = "0b00000000000000000000000000000000000000000000000000000000000000000";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_18")
{
    constexpr auto kSource = "0x1111222233334444";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_19")
{
    constexpr auto kSource = "0x11112222333344445";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}
