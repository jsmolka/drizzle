#pragma once

#include <vector>

#include "value.h"

struct Token
{
    enum class Type
    {
        And,
        AndAnd,
        Assert,
        Bang,
        BangEqual,
        BraceLeft,
        BraceRight,
        BracketLeft,
        BracketRight,
        Break,
        Caret,
        Class,
        Colon,
        Comma,
        Continue,
        Dedent,
        Dot,
        Elif,
        Else,
        Eof,
        Equal,
        EqualEqual,
        Extends,
        False,
        Float,
        For,
        Function,
        Greater,
        GreaterEqual,
        Identifier,
        If,
        In,
        Indent,
        Iterator,
        Integer,
        Less,
        LessEqual,
        Minus,
        NewLine,
        Noop,
        Null,
        ParenLeft,
        ParenRight,
        Percent,
        Pipe,
        PipePipe,
        Plus,
        Print,
        Return,
        Slash,
        SlashSlash,
        Star,
        String,
        Super,
        This,
        True,
        Var,
        While,
        Yield,
        LastEnumValue
    };

    Type type;
    Value value;
    std::size_t line;
    std::string_view lexeme;
};

using Tokens = std::vector<Token>;
