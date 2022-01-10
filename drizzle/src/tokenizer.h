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
  void newLine(bool emit = true);

  void emit(Token::Type type);
  void scanComment();
  void scanWhitespace();
  void scanBlankLines();
  void scanIndentation();
  void scanString();
  void scanNumber();
  void scanIdentifier();
  void scanToken();

  const char* begin;
  const char* cursor;
  const char* lexeme;
  std::size_t line = 0;
  std::size_t indentation = 0;
  std::vector<Token> tokens;
};
