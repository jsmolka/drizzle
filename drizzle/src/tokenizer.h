#pragma once

#include <vector>

#include "token.h"

class Tokenizer {
public:
  auto tokenize(const std::string& source) -> std::vector<Token>;

private:
  template<char kBase = 10>
    requires(kBase <= 36)
  static auto isDigit(char c) -> bool;
  static auto isAlpha(char c) -> bool;

  auto next() -> char;
  auto next(char match) -> bool;
  auto next(std::string_view match) -> bool;
  auto peek() const -> char;

  void emit(Token::Type type);
  void scanIndentation();
  void scanBlankLines();
  void scanWhitespace();
  void scanComment();
  void scanString();
  void scanNumber();
  void scanIdentifier();
  void scanToken();

  void parseInt();
  void parseFloat();
  void parseString();

  const char* cursor;
  const char* lexeme;
  std::size_t line;
  std::size_t indentation;
  std::vector<Token> tokens;
};
