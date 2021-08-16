#pragma once

#include <string>
#include <vector>

struct Token
{
    enum class Type
    {
        // Single
        BraceLeft, BraceRight, BracketLeft, BracketRight, Caret,
        Colon, Comma, Dot, Minus, ParenLeft, ParenRight, Percent,
        Plus, Slash, Star,

        // Single or double
        And, AndAnd, Bang, BangEqual, Equal, EqualEqual, Greater,
        GreaterEqual, Less, LessEqual, Pipe, PipePipe,

        // Literals
        Float, Identifier, Integer, String,
        
        // Keywords
        Break, Class, Continue, Elif, Else, Extends, False, For,
        Function, If, In, Iterator, Noop, Null, Return, Super,
        This, True, Var, While, Yield,

        // Special
        Eof, Indent, Dedent, NewLine,

        // Todo: remove when functions are implemented
        Print
    };

    Type type;
    std::size_t line;
    std::string_view lexeme;
};

class Scanner
{
public:
    std::vector<Token> scan(const std::string& source);

private:
    static bool isDigit(char c);
    static bool isAlpha(char c);

    bool isEof() const;
    char next();
    bool next(std::string_view match);
    char peek(std::size_t ahead = 0) const;

    void emit(Token::Type type);
    void scanIndentation();
    void scanBlankLines();
    void scanWhitespace();
    void scanComment();
    void scanString();
    void scanNumber();
    void scanToken();

    const char* cursor;
    const char* lexeme;
    std::size_t line;
    std::size_t indentation;
    std::vector<Token> tokens;
};
