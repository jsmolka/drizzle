#include "tokenizer.h"

#include <tuple>

#include <sh/parse.h>
#include <sh/utility.h>

#include "errors.h"

auto Tokenizer::tokenize(const std::string& source) -> std::vector<Token> {
  cursor = source.data();
  lexeme = source.data();
  line = 0;
  indentation = 0;
  tokens.clear();

  scanBlankLines();
  scanIndentation();

  while (*cursor) {
    lexeme = cursor;
    scanToken();
    scanWhitespace();
  }

  emit(Token::Type::Eof);

  return tokens;
}

template<char kBase>
  requires(kBase <= 36)
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
  const char* position = cursor;
  for (const auto& c : match) {
    if (!(*cursor && *cursor == c)) {
      cursor = position;
      return false;
    }
    cursor++;
  }
  return true;
}

auto Tokenizer::peek() const -> char {
  return *cursor ? cursor[1] : *cursor;
}

void Tokenizer::emit(Token::Type type) {
  Token token;
  token.type = type;
  token.line = line;
  token.lexeme = std::string_view(lexeme, std::distance(lexeme, cursor));
  tokens.push_back(token);

  lexeme = cursor;
}

void Tokenizer::scanIndentation() {
  constexpr auto kSpacesPerIndentation = 2;

  switch (*cursor) {
    case ' ':
    case '\r':
    case '\t':
      break;
    default:
      while (indentation > 0) {
        emit(Token::Type::Dedent);
        indentation--;
      }
      return;
  }

  const char* error = cursor;

  auto count_spaces = [this, error]() -> int {
    int spaces = 0;
    while (true) {
      switch (*cursor) {
        case ' ':
          next();
          spaces++;
          break;
        case '\r':
          next();
          break;
        case '\t':
          throw SyntaxError(error, "tabs used for indent");
        default:
          return spaces;
      }
    }
  };

  const auto spaces = count_spaces();
  if (spaces % kSpacesPerIndentation) {
    throw SyntaxError(error, "indent spaces must be a multiple of {}", kSpacesPerIndentation);
  }

  const auto indent = spaces / kSpacesPerIndentation;
  if (indent > indentation) {
    if ((indent - indentation) > 1) {
      throw SyntaxError(error, "multiple indents at once");
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

void Tokenizer::scanBlankLines() {
  auto skip_line = [this]() {
    const char* position = cursor;
    while (*cursor) {
      switch (*cursor) {
        case ' ':
        case '\r':
        case '\t':
          next();
          break;
        case '#':
          scanComment();
          break;
        case '\n':
          next();
          line++;
          return true;
        default:
          cursor = position;
          return false;
      }
    }
    return false;
  };

  while (skip_line()) {}
}

void Tokenizer::scanWhitespace() {
  while (true) {
    switch (*cursor) {
      case ' ':
      case '\r':
      case '\t':
        next();
        break;
      case '#':
        scanComment();
        break;
      case '\n':
        next();
        emit(Token::Type::NewLine);
        line++;
        scanBlankLines();
        scanIndentation();
        return;
      default:
        return;
    }
  }
}

void Tokenizer::scanComment() {
  while (*cursor && *cursor != '\n') {
    next();
  }
}

void Tokenizer::scanString() {
  const char* error = cursor;

  if (next(R"(""")")) {
    while (*cursor) {
      if (next(R"(""")")) {
        error = nullptr;
        break;
      }

      if (next() == '\n') {
        line++;
      }
    }
  } else {
    next();

    while (*cursor) {
      if (next('"')) {
        error = nullptr;
        break;
      }

      switch (*cursor) {
        case '\n':
          throw SyntaxError(cursor, "unexpected line break");
        case '\\':
          next();
          switch (*cursor) {
            case '\\':
            case '\"':
            case 'a':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
            case 'v':
              next();
              break;
            default:
              throw SyntaxError(cursor, "unknown escape sequence");
          }
          break;
        default:
          next();
          break;
      }
    }
  }

  if (error) {
    throw SyntaxError(error, "unterminated string");
  }

  emit(Token::Type::String);

  parseString();
}

void Tokenizer::scanNumber() {
  auto is_bin = isDigit<2>;
  auto is_dec = isDigit<10>;
  auto is_hex = isDigit<16>;

  if (next("0b")) {
    if (!is_bin(*cursor)) {
      throw SyntaxError(cursor, "expected bin digit");
    }

    while (is_bin(*cursor)) {
      next();
    }

    emit(Token::Type::Integer);
    parseInt();
    return;
  }

  if (next("0x")) {
    if (!is_hex(*cursor)) {
      throw SyntaxError(cursor, "expected hex digit");
    }

    while (is_hex(*cursor)) {
      next();
    }

    emit(Token::Type::Integer);
    parseInt();
    return;
  }

  if (*cursor == '0' && is_dec(peek())) {
    throw SyntaxError(cursor, "unexpected zero");
  }

  while (is_dec(*cursor)) {
    next();
  }

  if (next('.')) {
    if (!is_dec(*cursor)) {
      throw SyntaxError(cursor, "expected digit");
    }

    while (is_dec(*cursor)) {
      next();
    }

    emit(Token::Type::Float);
    parseFloat();
  } else {
    emit(Token::Type::Integer);
    parseInt();
  }
}

void Tokenizer::scanIdentifier() {
  static constexpr std::tuple<std::string_view, Token::Type> kKeywords[] = {
    {"block",    Token::Type::Block   },
    {"break",    Token::Type::Break   },
    {"continue", Token::Type::Continue},
    {"def",      Token::Type::Def     },
    {"elif",     Token::Type::Elif    },
    {"else",     Token::Type::Else    },
    {"false",    Token::Type::False   },
    {"if",       Token::Type::If      },
    {"noop",     Token::Type::Noop    },
    {"null",     Token::Type::Null    },
    {"print",    Token::Type::Print   },
    {"return",   Token::Type::Return  },
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
    return;
  }

  if (isDigit(*cursor)) {
    scanNumber();
    return;
  }

  if (*cursor == '"') {
    scanString();
    return;
  }

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
      if (next('>'))
        emit(next('>') ? Token::Type::Greater3 : Token::Type::Greater2);
      else
        emit(next('=') ? Token::Type::GreaterEqual : Token::Type::Greater);
      break;

    case '<':
      if (next('<'))
        emit(Token::Type::Less2);
      else if (next('='))
        emit(Token::Type::LessEqual);
      else
        emit(Token::Type::Less);
      break;

    default:
      throw SyntaxError(cursor - 1, "unexpected character");
  }
}

void Tokenizer::parseInt() {
  auto& token = tokens.back();
  if (const auto value = sh::parse<std::make_unsigned_t<dzint>>(token.lexeme)) {
    token.value = static_cast<dzint>(*value);
  } else {
    throw SyntaxError(token.lexeme.data(), "cannot parse int");
  }
}

void Tokenizer::parseFloat() {
  auto& token = tokens.back();
  if (const auto value = sh::parse<dzfloat>(token.lexeme)) {
    token.value = *value;
  } else {
    throw SyntaxError(token.lexeme.data(), "cannot parse float");
  }
}

void Tokenizer::parseString() {
  auto& token = tokens.back();
  auto lexeme = token.lexeme;
  auto quotes = lexeme.starts_with(R"(""")") ? 3 : 1;
  lexeme.remove_prefix(quotes);
  lexeme.remove_suffix(quotes);

  std::string value;
  value.reserve(lexeme.size());

  if (quotes == 1) {
    for (auto iter = lexeme.begin(); iter != lexeme.end(); ++iter) {
      switch (*iter) {
        case '\\':
          switch (*(++iter)) {
            case '\\': value.push_back('\\'); break;
            case '\"': value.push_back('\"'); break;
            case 'a':  value.push_back('\a'); break;
            case 'b':  value.push_back('\b'); break;
            case 'f':  value.push_back('\f'); break;
            case 'n':  value.push_back('\n'); break;
            case 'r':  value.push_back('\r'); break;
            case 't':  value.push_back('\t'); break;
            case 'v':  value.push_back('\v'); break;
            default:
              SH_UNREACHABLE;
              break;
          }
          break;
        default:
          value.push_back(*iter);
          break;
      }
    }
  } else {
    value = lexeme;
  }
  token.value = std::move(value);
}
