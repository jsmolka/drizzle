TEST_CASE("tokenizer_string_1")
{
    constexpr auto kSource = R"("""string""")";

    tokenizeValue<std::string>(kSource, "string");
}

TEST_CASE("tokenizer_string_2")
{
    constexpr auto kSource = R"("""string
""")";

    tokenizeValue<std::string>(kSource, "string\n");
}

TEST_CASE("tokenizer_string_3")
{
    constexpr auto kSource = R"("""string\""")";

    tokenizeValue<std::string>(kSource, "string\\");
}

TEST_CASE("tokenizer_string_4")
{
    constexpr auto kSource = R"("""string """)";

    tokenizeValue<std::string>(kSource, "string ");
}

TEST_CASE("tokenizer_string_5")
{
    constexpr auto kSource = R"("string")";

    tokenizeValue<std::string>(kSource, "string");
}

TEST_CASE("tokenizer_string_6")
{
    constexpr auto kSource = R"("\\ \" \a \b \f \n \r \t \v")";

    tokenizeValue<std::string>(kSource, "\\ \" \a \b \f \n \r \t \v");
}

TEST_CASE("tokenizer_string_7")
{
    constexpr auto kSource = "\"\n\"";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_string_8")
{
    std::vector<char> chars;
    for (int c = 0; c < 256; ++c)
    {
        switch (c)
        {
        case '\\':
        case '"':
        case 'a':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
        case 'v':
            continue;
        }

        chars.push_back(c);
    }

    for (const auto& c : chars)
    {
        const auto source = shell::format("\"\\{}\"", c);

        tokenizeThrows(source);
    }
}

TEST_CASE("tokenizer_string_9")
{
    constexpr auto kSource = "\"string";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_string_10")
{
    constexpr auto kSource = "\"\"\"string";

    tokenizeThrows(kSource);
}
