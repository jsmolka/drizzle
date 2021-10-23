TEST_CASE("tokenizer_number_1")
{
    constexpr auto kSource = "0b0";

    tokenizeValue<dzint>(kSource, 0b0);
}

TEST_CASE("tokenizer_number_2")
{
    constexpr auto kSource = "0b01001";

    tokenizeValue<dzint>(kSource, 0b01001);
}

TEST_CASE("tokenizer_number_3")
{
    constexpr auto kSource = "0b";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_number_4")
{
    constexpr auto kSource = "0b2";

    tokenizeThrows(kSource);
}
TEST_CASE("tokenizer_number_5")
{
    constexpr auto kSource = "0b_";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_number_6")
{
    constexpr auto kSource = "0x0";

    tokenizeValue<dzint>(kSource, 0x0);
}

TEST_CASE("tokenizer_number_7")
{
    constexpr auto kSource = "0xabcdef";

    tokenizeValue<dzint>(kSource, 0xabcdef);
}

TEST_CASE("tokenizer_number_8")
{
    constexpr auto kSource = "0xABCDEF";

    tokenizeValue<dzint>(kSource, 0xABCDEF);
}

TEST_CASE("tokenizer_number_9")
{
    constexpr auto kSource = "0x";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_number_10")
{
    constexpr auto kSource = "0xn";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_number_11")
{
    constexpr auto kSource = "0x_";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_number_12")
{
    constexpr auto kSource = "100";

    tokenizeValue<dzint>(kSource, 100);
}

TEST_CASE("tokenizer_number_13")
{
    constexpr auto kSource = "01";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_number_14")
{
    constexpr auto kSource = "1.1";

    tokenizeValue<dzfloat>(kSource, 1.1);
}

TEST_CASE("tokenizer_number_15")
{
    constexpr auto kSource = "01.1";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_number_16")
{
    constexpr auto kSource = "01.x1";

    tokenizeThrows(kSource);
}
