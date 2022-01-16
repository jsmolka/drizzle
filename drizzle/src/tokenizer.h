#pragma once

#include <optional>
#include <vector>

#include "token.h"

class Tokenizer {
public:
  auto tokenize(const std::string& source) -> std::vector<Token>;

private:
  template<char kBase = 10>
  static auto isDigit(char c) -> bool;
  static auto isAlpha(char c) -> bool;
  static auto isAlnum(char c) -> bool;

  auto next() -> char;
  auto next(char match) -> bool;
  auto next(std::string_view match) -> bool;
  auto peek() const -> char;

  auto cursorLocation() const -> Location;
  auto lexemeLocation() const -> Location;
  void emit(Token::Type type, std::optional<Location> location = std::nullopt);
  void skipComment();
  void skipWhitespace();
  void skipBlankLines();
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
