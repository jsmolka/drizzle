#pragma once

#include <sh/fmt.h>
#include <sh/utility.h>

#include "location.h"

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
    Class,
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
    Return,
    Slash,
    Slash2,
    Star,
    Star2,
    String,
    This,
    Tilde,
    True,
    Var,
    While,
    LastEnumValue,
  };

  Type type;
  std::string_view lexeme;
  Location location;
};

template<>
struct fmt::formatter<Token::Type> : fmt::formatter<std::string_view> {
  static_assert(int(Token::Type::LastEnumValue) == 57);
  template<typename FormatContext>
  auto format(const Token::Type& value, FormatContext& ctx) const {
    auto repr = [](const Token::Type& value) {
      switch (value) {
        case Token::Type::And:          return "And";
        case Token::Type::And2:         return "And2";
        case Token::Type::Bang:         return "Bang";
        case Token::Type::BangEqual:    return "BangEqual";
        case Token::Type::Block:        return "Block";
        case Token::Type::BraceLeft:    return "BraceLeft";
        case Token::Type::BraceRight:   return "BraceRight";
        case Token::Type::BracketLeft:  return "BracketLeft";
        case Token::Type::BracketRight: return "BracketRight";
        case Token::Type::Break:        return "Break";
        case Token::Type::Caret:        return "Caret";
        case Token::Type::Class:        return "Class";
        case Token::Type::Colon:        return "Colon";
        case Token::Type::Comma:        return "Comma";
        case Token::Type::Continue:     return "Continue";
        case Token::Type::Dedent:       return "Dedent";
        case Token::Type::Def:          return "Def";
        case Token::Type::Dot:          return "Dot";
        case Token::Type::Elif:         return "Elif";
        case Token::Type::Else:         return "Else";
        case Token::Type::Eof:          return "Eof";
        case Token::Type::Equal:        return "Equal";
        case Token::Type::Equal2:       return "Equal2";
        case Token::Type::False:        return "False";
        case Token::Type::Float:        return "Float";
        case Token::Type::Greater:      return "Greater";
        case Token::Type::Greater2:     return "Greater2";
        case Token::Type::Greater3:     return "Greater3";
        case Token::Type::GreaterEqual: return "GreaterEqual";
        case Token::Type::Identifier:   return "Identifier";
        case Token::Type::If:           return "If";
        case Token::Type::Indent:       return "Indent";
        case Token::Type::Integer:      return "Integer";
        case Token::Type::Less:         return "Less";
        case Token::Type::Less2:        return "Less2";
        case Token::Type::LessEqual:    return "LessEqual";
        case Token::Type::Minus:        return "Minus";
        case Token::Type::NewLine:      return "NewLine";
        case Token::Type::Noop:         return "Noop";
        case Token::Type::Null:         return "Null";
        case Token::Type::ParenLeft:    return "ParenLeft";
        case Token::Type::ParenRight:   return "ParenRight";
        case Token::Type::Percent:      return "Percent";
        case Token::Type::Pipe:         return "Pipe";
        case Token::Type::Pipe2:        return "Pipe2";
        case Token::Type::Plus:         return "Plus";
        case Token::Type::Return:       return "Return";
        case Token::Type::Slash:        return "Slash";
        case Token::Type::Slash2:       return "Slash2";
        case Token::Type::Star:         return "Star";
        case Token::Type::Star2:        return "Star2";
        case Token::Type::String:       return "String";
        case Token::Type::This:         return "This";
        case Token::Type::Tilde:        return "Tilde";
        case Token::Type::True:         return "True";
        case Token::Type::Var:          return "Var";
        case Token::Type::While:        return "While";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};
