#pragma once

#include <string>

#include "token.h"

class Scanner
{
public:
    Tokens scan(const std::string& source);

private:
    template<char Base = 10>
    static bool isDigit(char c);
    static bool isAlpha(char c);

    char next();
    bool next(char match);
    bool next(std::string_view match);
    char peek() const;

    void emit(Token::Type type);
    void scanIndentation();
    void scanBlankLines();
    void scanWhitespace();
    void scanComment();
    void scanString();
    void scanNumber();
    void scanIdentifier();
    void scanToken();

    const char* cursor;
    const char* lexeme;
    std::size_t indentation;
    Tokens tokens;
};
