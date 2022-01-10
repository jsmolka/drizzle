#pragma once

#include "astformatter.h"
#include "errors.h"
#include "parser.h"
#include "tokenizer.h"
#include "ut.h"

inline auto operator<<(std::ostream& out, const Token::Type& type) -> std::ostream& {
  return out << int(type);
}

inline void tokenize(const std::string& source, const std::vector<Token::Type>& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = Tokenizer().tokenize(source);
  expect(eq(tokens.size(), expected.size()), location);
  for (int i = 0; i < std::min(tokens.size(), expected.size()); ++i) {
    expect(eq(tokens[i].type, expected[i]), location);
  }
}

template<sh::any_of<dzint, dzfloat, std::string> T>
inline void tokenizeValue(const std::string& source, const T& value,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = Tokenizer().tokenize(source);
  for (const auto& token : tokens) {
    auto expected_token = []() constexpr -> Token::Type {
      if constexpr (dz_int<T>) {
        return Token::Type::Integer;
      } else if (dz_float<T>) {
        return Token::Type::Float;
      } else {
        return Token::Type::String;
      }
    };

    if (token.type == expected_token()) {
      expect(eq(std::get<T>(token.value), value), location);
      return;
    }
  }
  expect(false, location);
}

inline void tokenizeThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  expect(throws<SyntaxError>([&] {
    Tokenizer().tokenize(source);
  }), location);
}

inline void parse(const std::string& source, const std::string& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = Tokenizer().tokenize(source);
  const auto ast = Parser().parse(tokens);
  expect(eq(fmt::to_string(ast), expected), location);
}

inline void parseThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = Tokenizer().tokenize(source);
  expect(throws<SyntaxError>([&] {
    Parser().parse(tokens);
  }), location);
}

