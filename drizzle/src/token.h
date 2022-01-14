#pragma once

#include <string>
#include <variant>

#include "dzprimitives.h"
#include "sourcelocation.h"

struct Token {
  enum class Type {
    And,
    And2,
    Bang,
    BangEqual,
    Block,
    BraceLeft,
    BraceRight,
    BracketLeft,
    BracketRight,
    Break,
    Caret,
    Colon,
    Comma,
    Continue,
    Dedent,
    Def,
    Dot,
    Elif,
    Else,
    Eof,
    Equal,
    Equal2,
    False,
    Float,
    Greater,
    Greater2,
    Greater3,
    GreaterEqual,
    Identifier,
    If,
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
    Tilde,
    True,
    Var,
    While,
    LastEnumValue,
  };

  Type type;
  std::string_view lexeme;
  SourceLocation location;
};
