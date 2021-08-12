#pragma once

#include <string_view>
#include <vector>

#include "stack.h"

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
    std::string_view lexeme;
    std::size_t line;
};

class Scanner
{
public:
    void scan(std::string_view source);

    std::vector<Token> tokens;

private:
    void skipWhitespace();
    void skipEmptyLine();
    void skipComment();

    Token makeToken(Token::Type type) const;
    Token scanToken();

    char next();
    char peek() const;
    char peekNext() const;
    bool finished() const;
    bool match(char expected);

    const char* begin;
    const char* current;
    std::size_t line;
    bool line_begin;
    int level;
    Stack<int, 64> indentation;
};
