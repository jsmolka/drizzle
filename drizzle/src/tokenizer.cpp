#include "tokenizer.h"

#include <sh/utility.h>

#include "error.h"

auto Tokenizer::tokenize(const std::string& source) -> std::vector<Token> {
  begin = cursor = lexeme = source.data();

  scanIndentation();
  while (*cursor) {
    scanToken();
    skipWhitespace();
  }
  emit(Token::Type::Eof);

  return tokens;
}

template<char kBase>
auto Tokenizer::isDigit(char c) -> bool {
  if constexpr (kBase <= 10) {
    return (c >= '0') && (c < ('0' + kBase));
  } else if constexpr (kBase <= 36) {
    return (c >= '0' && c <= '9')
        || (c >= 'a' && c < ('a' + kBase - 10))
        || (c >= 'A' && c < ('A' + kBase - 10));
  }
}

auto Tokenizer::isAlpha(char c) -> bool {
  return (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || (c == '_');
}

auto Tokenizer::isAlnum(char c) -> bool {
  return (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || (c >= '0' && c <= '9');
}

auto Tokenizer::next() -> char {
  cursor++;
  return cursor[-1];
}

auto Tokenizer::next(char match) -> bool {
  const auto matched = *cursor && *cursor == match;
  if (matched) {
    cursor++;
  }
  return matched;
}

auto Tokenizer::next(std::string_view match) -> bool {
  const auto previous = cursor;
  for (const auto& c : match) {
    if (!(*cursor && *cursor == c)) {
      cursor = previous;
      return false;
    }
    cursor++;
  }
  return true;
}

auto Tokenizer::peek() const -> char {
  return *cursor ? cursor[1] : *cursor;
}

auto Tokenizer::cursorLocation() const -> Location {
  assert(cursor - begin >= 0);
  return Location{
    .line = line,
    .column = cursor - begin
  };
}

auto Tokenizer::lexemeLocation() const -> Location {
  assert(lexeme - begin >= 0);
  return Location{
    .line = line,
    .column = lexeme - begin
  };
}

void Tokenizer::emit(Token::Type type, std::optional<Location> location) {
  tokens.push_back(Token{
    .type = type,
    .lexeme = std::string_view(lexeme, std::distance(lexeme, cursor)),
    .location = location ? *location : lexemeLocation()
  });
  lexeme = cursor;
}

void Tokenizer::skipComment() {
  while (*cursor && *cursor != '\n') {
    next();
  }
}

void Tokenizer::skipWhitespace() {
  auto skip = [this] {
    while (true) {
      switch (*cursor) {
        case ' ':
        case '\t':
        case '\r':
          next();
          break;
        case '#':
          skipComment();
          break;
        case '\n':
          next();
          [[fallthrough]];
        case '\0':
          emit(Token::Type::NewLine);
          begin = cursor;
          line++;
          scanIndentation();
          [[fallthrough]];
        default:
          return;
      }
    }
  };

  skip();
  lexeme = cursor;
}

void Tokenizer::skipBlankLines() {
  assert(cursor == begin);

  auto skip = [this] {
    while (true) {
      switch (*cursor) {
        case ' ':
        case '\r':
        case '\t':
          next();
          break;
        case '#':
          skipComment();
          break;
        case '\n':
          next();
          [[fallthrough]];
        case '\0':
          begin = cursor;
          line++;
          return true;
        default:
          return false;
      }
    }
  };

  while (*cursor && skip());
  lexeme = cursor = begin;
}

void Tokenizer::scanIndentation() {
  assert(cursor == begin);

  skipBlankLines();

  switch (*cursor) {
    case ' ':
      break;
    case '\t':
      throw SyntaxError(lexemeLocation(), "tab used for indent");
    default:
      while (indentation > 0) {
        emit(Token::Type::Dedent);
        indentation--;
      }
      return;
  }

  auto spaces = 0;
  while (*cursor == ' ') {
    spaces++;
    next();
  }

  if (spaces % 2) {
    throw SyntaxError(lexemeLocation(), "indent spaces must be a multiple of two");
  }

  const auto indent = spaces / 2;
  if (indent > indentation) {
    if (indent - indentation > 1) {
      throw SyntaxError(lexemeLocation(), "multiple indents at once");
    }

    emit(Token::Type::Indent);
    indentation++;
  } else {
    while (indent < indentation) {
      emit(Token::Type::Dedent);
      indentation--;
    }
  }
}

void Tokenizer::scanString() {
  const auto location = lexemeLocation();

  auto terminated = false;
  if (next(R"(""")")) {
    while (*cursor && !(terminated = next(R"(""")"))) {
      line += next() == '\n';
    }
  } else if (next(R"(")")) {
    while (*cursor && !(terminated = next(R"(")"))) {
      switch (next()) {
        case '\0':
        case '\n':
          throw SyntaxError(cursorLocation(), "expected closing quote");

        case '\\':
          switch (*cursor) {
            case '\\':
            case '"':
            case 'n':
            case 'r':
            case 't':
              next();
              break;
            default:
              throw SyntaxError(cursorLocation(), "unexpected escape sequence");
          }
          break;
      }
    }
  } else {
    SH_UNREACHABLE;
  }

  if (!terminated) {
    throw SyntaxError(location, "unterminated string");
  }

  emit(Token::Type::String, location);
}

void Tokenizer::scanNumber() {
  using IsDigit = bool(*)(char);

  IsDigit is_digit;
  if (next("0b")) {
    is_digit = isDigit<2>;
  } else if (next("0x")) {
    is_digit = isDigit<16>;
  } else {
    is_digit = isDigit<10>;
    if (*cursor == '0' && isAlnum(peek())) {
      throw SyntaxError(cursorLocation(), "unexpected leading zero");
    }
  }

  auto scan = [&] {
    do {
      if (!is_digit(*cursor)) {
        throw SyntaxError(cursorLocation(), "expected digit");
      }
      next();
    } while (isAlnum(*cursor));
  };

  scan();

  if (is_digit == isDigit<10> && next('.')) {
    scan();
    emit(Token::Type::Float);
  } else {
    emit(Token::Type::Integer);
  }
}

void Tokenizer::scanIdentifier() {
  static constexpr std::tuple<std::string_view, Token::Type> kKeywords[] = {
    {"block",    Token::Type::Block   },
    {"break",    Token::Type::Break   },
    {"class",    Token::Type::Class   },
    {"continue", Token::Type::Continue},
    {"def",      Token::Type::Def     },
    {"elif",     Token::Type::Elif    },
    {"else",     Token::Type::Else    },
    {"false",    Token::Type::False   },
    {"if",       Token::Type::If      },
    {"in",       Token::Type::In      },
    {"noop",     Token::Type::Noop    },
    {"null",     Token::Type::Null    },
    {"return",   Token::Type::Return  },
    {"this",     Token::Type::This    },
    {"true",     Token::Type::True    },
    {"var",      Token::Type::Var     },
    {"while",    Token::Type::While   },
  };

  while (isAlpha(*cursor) || isDigit(*cursor)) {
    next();
  }

  std::string_view identifier(lexeme, std::distance(lexeme, cursor));
  for (const auto& [keyword, token] : kKeywords) {
    if (identifier == keyword) {
      emit(token);
      return;
    }
  }
  emit(Token::Type::Identifier);
}

void Tokenizer::scanToken() {
  if (isAlpha(*cursor)) {
    scanIdentifier();
  } else if (isDigit(*cursor)) {
    scanNumber();
  } else if (*cursor == '"') {
    scanString();
  } else {
    switch (next()) {
      case '{': emit(Token::Type::BraceLeft); break; 
      case '}': emit(Token::Type::BraceRight); break;
      case '[': emit(Token::Type::BracketLeft); break;
      case ']': emit(Token::Type::BracketRight); break;
      case '^': emit(Token::Type::Caret); break;
      case ':': emit(Token::Type::Colon); break;
      case ',': emit(Token::Type::Comma); break;
      case '.': emit(Token::Type::Dot); break;
      case '-': emit(Token::Type::Minus); break;
      case '(': emit(Token::Type::ParenLeft); break;
      case ')': emit(Token::Type::ParenRight); break;
      case '%': emit(Token::Type::Percent); break;
      case '+': emit(Token::Type::Plus); break;
      case '~': emit(Token::Type::Tilde); break;

      case '&': emit(next('&') ? Token::Type::And2 : Token::Type::And); break;
      case '!': emit(next('=') ? Token::Type::BangEqual : Token::Type::Bang); break;
      case '=': emit(next('=') ? Token::Type::Equal2 : Token::Type::Equal); break;
      case '|': emit(next('|') ? Token::Type::Pipe2 : Token::Type::Pipe); break;
      case '/': emit(next('/') ? Token::Type::Slash2 : Token::Type::Slash); break;
      case '*': emit(next('*') ? Token::Type::Star2 : Token::Type::Star); break;

      case '>':
        if (next('>')) {
          emit(next('>') ? Token::Type::Greater3 : Token::Type::Greater2);
        } else {
          emit(next('=') ? Token::Type::GreaterEqual : Token::Type::Greater);
        }
        break;

      case '<':
        if (next('<')) {
          emit(Token::Type::Less2);
        } else if (next('=')) {
          emit(Token::Type::LessEqual);
        } else {
          emit(Token::Type::Less);
        }
        break;

      default:
        throw SyntaxError(lexemeLocation(), "unexpected character");
    }
  }
}
