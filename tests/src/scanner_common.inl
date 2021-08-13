void require(std::string_view source, const std::vector<Token::Type>& expected)
{
    Scanner scanner;
    const auto tokens = scanner.scan(source);
    REQUIRE(tokens.size() == expected.size());

    for (auto [token, type] : shell::zip(tokens, expected))
        REQUIRE(token.type == type);
}
