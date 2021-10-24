TEST_CASE("tokenizer_indentation_1")
{
    constexpr auto kSource = R"(
noop # 1
  noop # 2
    noop # 3
  noop # 4
noop # 5
)";

    tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Noop,  // 4
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Noop,  // 5
        Token::Type::NewLine,
        Token::Type::Eof 
    });
}

TEST_CASE("tokenizer_indentation_2")
{
    constexpr auto kSource = R"(
noop # 1
  noop # 2
  noop # 3
noop # 4
)";

    tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Noop,  // 4
        Token::Type::NewLine,
        Token::Type::Eof 
    });
}

TEST_CASE("tokenizer_indentation_3")
{
    constexpr auto kSource = R"(
noop # 1
  noop # 2
    noop # 3
)";

    tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Dedent,
        Token::Type::Eof 
    });
}

TEST_CASE("tokenizer_indentation_4")
{
    constexpr auto kSource = R"(
noop # 1
  noop # 2
    noop # 3
    noop # 4
)";

    tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Noop,  // 4
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Dedent,
        Token::Type::Eof 
    });
}

TEST_CASE("tokenizer_indentation_5")
{
    constexpr auto kSource = "\r  noop\n";

    tokenize(kSource, { 
        Token::Type::Indent,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Eof 
    });
}

TEST_CASE("tokenizer_indentation_6")
{
    constexpr auto kSource = "\tnoop";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_indentation_7")
{
    constexpr auto kSource = " noop";

    tokenizeThrows(kSource);
}

TEST_CASE("tokenizer_indentation_8")
{
    constexpr auto kSource = "    noop";

    tokenizeThrows(kSource);
}
