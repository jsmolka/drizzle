#pragma once

#include <string>
#include <variant>
#include <vector>

#include "dzprimitives.h"

class Token
{
public:
    enum class Type
    {
        And,
        And2,
        Assert,
        Bang,
        BangEqual,
        Block,
        BraceLeft,
        BraceRight,
        BracketLeft,
        BracketRight,
        Break,
        Caret,
        Case,
        Class,
        Colon,
        Comma,
        Continue,
        Dedent,
        Define,
        Dot,
        Elif,
        Else,
        Eof,
        Equal,
        Equal2,
        Extends,
        False,
        Float,
        For,
        Greater,
        Greater2,
        Greater3,
        GreaterEqual,
        Identifier,
        If,
        In,
        Indent,
        Integer,
        Less,
        Less2,
        LessEqual,
        Minus,
        NewLine,
        Noop,
        Null,
        ParenLeft,
        ParenRight,
        Percent,
        Pipe,
        Pipe2,
        Plus,
        Print,
        Return,
        Slash,
        Slash2,
        Star,
        Star2,
        String,
        Super,
        Switch,
        This,
        Tilde,
        True,
        Var,
        While,
        Yield,
        LastEnumValue
    };

    Type type;
    std::size_t line;
    std::string_view lexeme;
    std::variant<dzint, dzfloat, std::string> value;
};

using Tokens = std::vector<Token>;
