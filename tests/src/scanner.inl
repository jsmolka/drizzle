void scan(const std::string& source, const std::vector<Token::Type>& expected)
{
    Scanner scanner;
    const auto tokens = scanner.scan(source);
    REQUIRE(tokens.size() == expected.size());

    for (auto [token, type] : shell::zip(tokens, expected))
        REQUIRE(token.type == type);
}

#include "scanner_blank_lines.inl"
#include "scanner_comments.inl"
#include "scanner_eof.inl"
#include "scanner_indentation.inl"
#include "scanner_string.inl"
#include "scanner_tokens_double.inl"
#include "scanner_tokens_single.inl"
#include "scanner_whitespace.inl"
