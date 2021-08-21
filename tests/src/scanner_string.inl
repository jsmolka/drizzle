TEST_CASE("scanner_string_1")
{
    constexpr auto kSource = R"("""xxx""")";

    scanLexeme(kSource, Token::Type::String, kSource);
}

TEST_CASE("scanner_string_2")
{
    constexpr auto kSource = R"(  """xxx""")";

    scanLexeme(kSource, Token::Type::String, R"("""xxx""")");
}

TEST_CASE("scanner_string_3")
{
    constexpr auto kSource = R"("""xxx"""  )";

    scanLexeme(kSource, Token::Type::String, R"("""xxx""")");
}

TEST_CASE("scanner_string_4")
{
    constexpr auto kSource = R"(  """xxx"""  )";

    scanLexeme(kSource, Token::Type::String, R"("""xxx""")");
}

TEST_CASE("scanner_string_5")
{
    constexpr auto kSource = R"("""
""")";

    scanLexeme(kSource, Token::Type::String, kSource);
}

TEST_CASE("scanner_string_6")
{
    constexpr auto kSource = R"("""\""")";

    scanLexeme(kSource, Token::Type::String, kSource);
}

TEST_CASE("scanner_string_7")
{
    constexpr auto kSource = R"("""     """)";

    scanLexeme(kSource, Token::Type::String, kSource);
}

TEST_CASE("scanner_string_8")
{
    constexpr auto kSource = R"("""xxx"")";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_string_9")
{
    constexpr auto kSource = R"("""xxx)";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_string_10")
{
    constexpr auto kSource = R"(var x = """xxx""")";

    scanLexeme(kSource, Token::Type::String, R"("""xxx""")");
}

TEST_CASE("scanner_string_11")
{
    constexpr auto kSource = R"(var x = """xxx
""")";

    scanLexeme(kSource, Token::Type::String, R"("""xxx
""")");
}

TEST_CASE("scanner_string_12")
{
    constexpr auto kSource = R"(var x ="""xxx
""")";

    scanLexeme(kSource, Token::Type::String, R"("""xxx
""")");
}

TEST_CASE("scanner_string_13")
{
    constexpr auto kSource = R"("xxx")";

    scanLexeme(kSource, Token::Type::String, kSource);
}

TEST_CASE("scanner_string_14")
{
    constexpr auto kSource = R"(  "xxx")";

    scanLexeme(kSource, Token::Type::String, R"("xxx")");
}

TEST_CASE("scanner_string_15")
{
    constexpr auto kSource = R"("xxx"  )";

    scanLexeme(kSource, Token::Type::String, R"("xxx")");
}

TEST_CASE("scanner_string_16")
{
    constexpr auto kSource = R"(  "xxx"  )";

    scanLexeme(kSource, Token::Type::String, R"("xxx")");
}

TEST_CASE("scanner_string_17")
{
    constexpr auto kSource = R"("\\ \" \a \b \f \n \r \t \v")";

    scanLexeme(kSource, Token::Type::String, kSource);
}

TEST_CASE("scanner_string_18")
{
    constexpr auto kSource = R"("\x")";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_string_19")
{
    constexpr auto kSource = R"("\y")";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_string_20")
{
    constexpr auto kSource = R"("xxx)";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_string_21")
{
    constexpr auto kSource = R"("
xxx")";

    Scanner scanner;
    CHECK_THROWS_AS(scanner.scan(kSource), SyntaxError);
}

TEST_CASE("scanner_string_22")
{
    constexpr auto kSource = R"(var x = "xxx")";

    scanLexeme(kSource, Token::Type::String, R"("xxx")");
}

TEST_CASE("scanner_string_23")
{
    constexpr auto kSource = R"(var x ="xxx")";

    scanLexeme(kSource, Token::Type::String, R"("xxx")");
}
