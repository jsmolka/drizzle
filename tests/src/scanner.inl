void scan(const std::string& source, const std::vector<Token::Type>& expected)
{
    Scanner scanner;
    const auto tokens = scanner.scan(source);
    REQUIRE(tokens.size() == expected.size());

    for (auto [token, type] : shell::zip(tokens, expected))
        REQUIRE(token.type == type);
}

void scanLexeme(const std::string& source, Token::Type type, const std::string& lexeme)
{
    Scanner scanner;
    auto tokens = scanner.scan(source);
    for (const auto& token : tokens)
    {
        if (token.type == type)
        {
            REQUIRE(token.lexeme == lexeme);
            return;
        }
    }
    REQUIRE(false);
}

void scanThrows(const std::string& source)
{
    Scanner scanner;
    REQUIRE_THROWS_AS(scanner.scan(source), SyntaxError);
}

#include "scanner_blank_lines.inl"
#include "scanner_comments.inl"
#include "scanner_eof.inl"
#include "scanner_identifiers.inl"
#include "scanner_indentation.inl"
#include "scanner_number.inl"
#include "scanner_string.inl"
#include "scanner_tokens.inl"
#include "scanner_whitespace.inl"
