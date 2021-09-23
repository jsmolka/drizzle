#pragma once

#include <string>
#include <variant>
#include <vector>

#include "dztypes.h"

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
        Equal2,
        Extends,
        False,
        Float,
        For,
        Function,
        Greater,
        Greater2,
        Greater3,
        GreaterEqual,
        Identifier,
        If,
        In,
        Indent,
        Iterator,
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
