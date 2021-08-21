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
    constexpr auto kSource = "100";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_17")
{
    constexpr auto kSource = "  100";

    scanLexeme(kSource, Token::Type::Integer, "100");
}

TEST_CASE("scanner_number_18")
{
    constexpr auto kSource = "  100  ";

    scanLexeme(kSource, Token::Type::Integer, "100");
}

TEST_CASE("scanner_number_19")
{
    constexpr auto kSource = "var x = 100";

    scanLexeme(kSource, Token::Type::Integer, "100");
}

TEST_CASE("scanner_number_20")
{
    constexpr auto kSource = "var x =100";

    scanLexeme(kSource, Token::Type::Integer, "100");
}

TEST_CASE("scanner_number_21")
{
    constexpr auto kSource = "01";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_22")
{
    constexpr auto kSource = "1.1";

    scanLexeme(kSource, Token::Type::Float, kSource);
}

TEST_CASE("scanner_number_23")
{
    constexpr auto kSource = "  1.1";

    scanLexeme(kSource, Token::Type::Float, "1.1");
}

TEST_CASE("scanner_number_24")
{
    constexpr auto kSource = "  1.1  ";

    scanLexeme(kSource, Token::Type::Float, "1.1");
}

TEST_CASE("scanner_number_25")
{
    constexpr auto kSource = "var x = 1.1";

    scanLexeme(kSource, Token::Type::Float, "1.1");
}

TEST_CASE("scanner_number_26")
{
    constexpr auto kSource = "var x =1.1";

    scanLexeme(kSource, Token::Type::Float, "1.1");
}

TEST_CASE("scanner_number_27")
{
    constexpr auto kSource = "01.1";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_number_28")
{
    constexpr auto kSource = "01.x1";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}
