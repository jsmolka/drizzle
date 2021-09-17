TEST_CASE("scanner_number_1")
{
    constexpr auto kSource = "0b0";

    auto token = scanLexeme(kSource, Token::Type::Integer, kSource);
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0b0);
}

TEST_CASE("scanner_number_2")
{
    constexpr auto kSource = "0b01001";

    auto token = scanLexeme(kSource, Token::Type::Integer, kSource);
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0b01001);
}

TEST_CASE("scanner_number_3")
{
    constexpr auto kSource = "var x = 0b01001";

    auto token = scanLexeme(kSource, Token::Type::Integer, "0b01001");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0b01001);
}

TEST_CASE("scanner_number_4")
{
    constexpr auto kSource = "var x=0b01001\n";

    auto token = scanLexeme(kSource, Token::Type::Integer, "0b01001");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0b01001);
}

TEST_CASE("scanner_number_5")
{
    constexpr auto kSource = "0b";

    scanThrows(kSource);
}

TEST_CASE("scanner_number_6")
{
    constexpr auto kSource = "0b2";

    scanThrows(kSource);
}
TEST_CASE("scanner_number_7")
{
    constexpr auto kSource = "0b_";

    scanThrows(kSource);
}

TEST_CASE("scanner_number_8")
{
    constexpr auto kSource = "0x0";

    scanLexeme(kSource, Token::Type::Integer, kSource);
}

TEST_CASE("scanner_number_9")
{
    constexpr auto kSource = "0xabcdef";

    auto token = scanLexeme(kSource, Token::Type::Integer, kSource);
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0xABCDEF);
}

TEST_CASE("scanner_number_10")
{
    constexpr auto kSource = "0xABCDEF";

    auto token = scanLexeme(kSource, Token::Type::Integer, kSource);
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0xABCDEF);
}

TEST_CASE("scanner_number_11")
{
    constexpr auto kSource = "var x = 0xDEAD";

    auto token = scanLexeme(kSource, Token::Type::Integer, "0xDEAD");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0xDEAD);
}

TEST_CASE("scanner_number_12")
{
    constexpr auto kSource = "var x=0xDEAD\n";

    auto token = scanLexeme(kSource, Token::Type::Integer, "0xDEAD");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 0xDEAD);
}

TEST_CASE("scanner_number_13")
{
    constexpr auto kSource = "0x";

    scanThrows(kSource);
}

TEST_CASE("scanner_number_14")
{
    constexpr auto kSource = "0xn";

    scanThrows(kSource);
}

TEST_CASE("scanner_number_15")
{
    constexpr auto kSource = "0x_";

    scanThrows(kSource);
}

TEST_CASE("scanner_number_16")
{
    constexpr auto kSource = "100";

    auto token = scanLexeme(kSource, Token::Type::Integer, kSource);
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 100);
}

TEST_CASE("scanner_number_17")
{
    constexpr auto kSource = "  100";

    auto token = scanLexeme(kSource, Token::Type::Integer, "100");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 100);
}

TEST_CASE("scanner_number_18")
{
    constexpr auto kSource = "  100  ";

    auto token = scanLexeme(kSource, Token::Type::Integer, "100");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 100);
}

TEST_CASE("scanner_number_19")
{
    constexpr auto kSource = "var x = 100";

    auto token = scanLexeme(kSource, Token::Type::Integer, "100");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 100);
}

TEST_CASE("scanner_number_20")
{
    constexpr auto kSource = "var x =100";

    auto token = scanLexeme(kSource, Token::Type::Integer, "100");
    REQUIRE(token.value.type == Value::Type::Int);
    REQUIRE(token.value.i == 100);
}

TEST_CASE("scanner_number_21")
{
    constexpr auto kSource = "01";

    scanThrows(kSource);
}

TEST_CASE("scanner_number_22")
{
    constexpr auto kSource = "1.1";

    auto token = scanLexeme(kSource, Token::Type::Float, kSource);
    REQUIRE(token.value.type == Value::Type::Float);
    REQUIRE(token.value.f == 1.1);
}

TEST_CASE("scanner_number_23")
{
    constexpr auto kSource = "  1.1";

    auto token = scanLexeme(kSource, Token::Type::Float, "1.1");
    REQUIRE(token.value.type == Value::Type::Float);
    REQUIRE(token.value.f == 1.1);
}

TEST_CASE("scanner_number_24")
{
    constexpr auto kSource = "  1.1  ";

    auto token = scanLexeme(kSource, Token::Type::Float, "1.1");
    REQUIRE(token.value.type == Value::Type::Float);
    REQUIRE(token.value.f == 1.1);
}

TEST_CASE("scanner_number_25")
{
    constexpr auto kSource = "var x = 1.1";

    auto token = scanLexeme(kSource, Token::Type::Float, "1.1");
    REQUIRE(token.value.type == Value::Type::Float);
    REQUIRE(token.value.f == 1.1);
}

TEST_CASE("scanner_number_26")
{
    constexpr auto kSource = "var x =1.1";

    auto token = scanLexeme(kSource, Token::Type::Float, "1.1");
    REQUIRE(token.value.type == Value::Type::Float);
    REQUIRE(token.value.f == 1.1);
}

TEST_CASE("scanner_number_27")
{
    constexpr auto kSource = "01.1";

    scanThrows(kSource);
}

TEST_CASE("scanner_number_28")
{
    constexpr auto kSource = "01.x1";

    scanThrows(kSource);
}
