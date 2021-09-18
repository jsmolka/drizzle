#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "value.h"

struct Token
{
    enum class Type
    {
        // Single
        BraceLeft, BraceRight, BracketLeft, BracketRight, Caret,
        Colon, Comma, Dot, Minus, ParenLeft, ParenRight, Percent,
        Plus, Star,

        // Single or double
        And, AndAnd, Bang, BangEqual, Equal, EqualEqual, Greater,
        GreaterEqual, Less, LessEqual, Pipe, PipePipe, Slash,
        SlashSlash,

        // Literals
        Float, Identifier, Integer, String,

        // Keywords
        Break, Class, Continue, Elif, Else, Extends, False, For,
        Function, If, In, Iterator, Noop, Null, Return, Super,
        This, True, Var, While, Yield,

        // Special
        Eof, Indent, Dedent, NewLine,

        // Todo: remove when functions are implemented
        Assert, Print,

        LastEnumValue
    };

    Type type;
    Value value;
    std::size_t line;
    std::string_view lexeme;
};

using Tokens = std::vector<Token>;

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

    void parseInt(int base);
    void parseFloat();

    const char* cursor;
    const char* lexeme;
    std::size_t line;
    std::size_t indentation;
    Tokens tokens;
};
