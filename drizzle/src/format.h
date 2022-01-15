#pragma once

#include <sh/fmt.h>
#include <sh/utility.h>

#include "dzobject.h"
#include "dzstring.h"
#include "dzvalue.h"
#include "expression.h"
#include "statement.h"
#include "token.h"

template<>
struct fmt::formatter<Token::Type> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Token::Type& value, FormatContext& ctx) {
    static_assert(int(Token::Type::LastEnumValue) == 56);
    auto repr = [](const Token::Type& value) -> std::string_view {
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
        case Token::Type::Print:        return "Print";
        case Token::Type::Return:       return "Return";
        case Token::Type::Slash:        return "Slash";
        case Token::Type::Slash2:       return "Slash2";
        case Token::Type::Star:         return "Star";
        case Token::Type::Star2:        return "Star2";
        case Token::Type::String:       return "String";
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

template<>
struct fmt::formatter<Expression::Type> : fmt::formatter<std::string_view> {
  static constexpr auto repr(const Expression::Type& value) -> std::string_view {
    static_assert(int(Expression::Type::LastEnumValue) == 6);
    switch (value) {
      case Expression::Type::Assign:   return "assign";
      case Expression::Type::Binary:   return "binary";
      case Expression::Type::Group:    return "group";
      case Expression::Type::Literal:  return "literal";
      case Expression::Type::Unary:    return "unary";
      case Expression::Type::Variable: return "variable";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }

  template<typename FormatContext>
  auto format(const Expression::Type& value, FormatContext& ctx) {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<Expression::Binary::Type> : fmt::formatter<std::string_view> {
  static constexpr auto repr(const Expression::Binary::Type& value) -> std::string_view {
    static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);
    switch (value) {
      case Expression::Binary::Type::Addition:        return "+";
      case Expression::Binary::Type::And:             return "&&";
      case Expression::Binary::Type::BitwiseAnd:      return "&";
      case Expression::Binary::Type::BitwiseAsr:      return ">>";
      case Expression::Binary::Type::BitwiseLsl:      return "<<";
      case Expression::Binary::Type::BitwiseLsr:      return ">>>";
      case Expression::Binary::Type::BitwiseOr:       return "|";
      case Expression::Binary::Type::BitwiseXor:      return "^";
      case Expression::Binary::Type::Division:        return "/";
      case Expression::Binary::Type::Equal:           return "==";
      case Expression::Binary::Type::Greater:         return ">";
      case Expression::Binary::Type::GreaterEqual:    return ">=";
      case Expression::Binary::Type::IntegerDivision: return "//";
      case Expression::Binary::Type::Less:            return "<";
      case Expression::Binary::Type::LessEqual:       return "<=";
      case Expression::Binary::Type::Modulo:          return "%";
      case Expression::Binary::Type::Multiplication:  return "*";
      case Expression::Binary::Type::NotEqual:        return "!=";
      case Expression::Binary::Type::Or:              return "||";
      case Expression::Binary::Type::Power:           return "**";
      case Expression::Binary::Type::Subtraction:     return "-";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }

  template<typename FormatContext>
  auto format(const Expression::Binary::Type& value, FormatContext& ctx) {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<Expression::Literal> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const Expression::Literal& value, FormatContext& ctx) {
    static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);
    switch (value.type) {
      case Expression::Literal::Type::Boolean: return fmt::format_to(ctx.out(), "{}", std::get<dzbool>(value.value));
      case Expression::Literal::Type::Float:   return fmt::format_to(ctx.out(), "{}", std::get<dzfloat>(value.value));
      case Expression::Literal::Type::Integer: return fmt::format_to(ctx.out(), "{}", std::get<dzint>(value.value));
      case Expression::Literal::Type::Null:    return fmt::format_to(ctx.out(), "null");
      case Expression::Literal::Type::String:  return fmt::format_to(ctx.out(), R"("{}")", std::get<std::string>(value.value));
      default:
        SH_UNREACHABLE;
        return fmt::format_to(ctx.out(), "unreachable");
    }
  }
};

template<>
struct fmt::formatter<Expression::Unary::Type> : fmt::formatter<std::string_view> {
  static constexpr auto repr(const Expression::Unary::Type& value) -> std::string_view {
    static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);
    switch (value) {
      case Expression::Unary::Type::BitwiseComplement: return "~";
      case Expression::Unary::Type::Minus:             return "-";
      case Expression::Unary::Type::Not:               return "!";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }

  template<typename FormatContext>
  auto format(const Expression::Unary::Type& value, FormatContext& ctx) {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<Statement::Type> : fmt::formatter<std::string_view> {
  static constexpr auto repr(const Statement::Type& value) -> std::string_view {
    static_assert(int(Statement::Type::LastEnumValue) == 10);
    switch (value) {
      case Statement::Type::Block:               return "block";
      case Statement::Type::Break:               return "break";
      case Statement::Type::Continue:            return "continue";
      case Statement::Type::ExpressionStatement: return "expression_statement";
      case Statement::Type::If:                  return "if";
      case Statement::Type::Noop:                return "noop";
      case Statement::Type::Print:               return "print";
      case Statement::Type::Program:             return "program";
      case Statement::Type::Var:                 return "var";
      case Statement::Type::While:               return "while";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }

  template<typename FormatContext>
  auto format(const Statement::Type& value, FormatContext& ctx) {
    return fmt::formatter<std::string_view>::format(repr(value), ctx);
  }
};

template<>
struct fmt::formatter<DzValue> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const DzValue& value, FormatContext& ctx) {
    auto whole = [](double v) {
      return std::fmod(v, 1.0) == 0.0;
    };

    switch (value.type) {
      case DzValue::Type::Bool:   return fmt::format_to(ctx.out(), "{}", value.b);
      case DzValue::Type::Int:    return fmt::format_to(ctx.out(), "{}", value.i);
      case DzValue::Type::Float:  return fmt::format_to(ctx.out(), fmt::runtime(whole(value.f) ? "{:.1f}" : "{}"), value.f);
      case DzValue::Type::Object: return fmt::format_to(ctx.out(), "{}", *value.o);
      case DzValue::Type::Null:   return fmt::format_to(ctx.out(), "null");
      default:
        SH_UNREACHABLE;
        return fmt::format_to(ctx.out(), "unreachable");
    }
  }
};

template<>
struct fmt::formatter<DzObject> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const DzObject& object, FormatContext& ctx) {
    switch (object.type) {
      case DzObject::Type::String:
        return fmt::format_to(ctx.out(), "{}", static_cast<const DzString*>(&object)->data);
      default:
        SH_UNREACHABLE;
        return fmt::format_to(ctx.out(), "unreachable");
    }
  }
};
