#pragma once

#include <astformatter.h>
#include <errors.h>
#include <parser.h>
#include <tokenizer.h>

#include "ut.h"

auto operator<<(std::ostream& out, const Token::Type& type) -> std::ostream& {
  return out << int(type);
}

void tokenize(const std::string& source, const std::vector<Token::Type>& expected) {
  const auto tokens = Tokenizer().tokenize(source);
  const auto size = std::min(tokens.size(), expected.size());

  for (int i = 0; i < size; ++i) {
    expect(eq(tokens[i].type, expected[i]));
  }
  expect(eq(tokens.size(), expected.size()));
}

template<sh::any_of<dzint, dzfloat, std::string> T>
void tokenizeValue(const std::string& source, const T& value) {
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
      expect(eq(std::get<T>(token.value), value));
      return;
    }
  }
  expect(false);
}

void tokenizeThrows(const std::string& source) {
  expect(throws<SyntaxError>([&] {
    Tokenizer().tokenize(source);
  }));
}

inline void parse(std::string source, const std::string& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  source.append("\n\n");
  const auto tokens = Tokenizer().tokenize(source);
  const auto ast = Parser().parse(tokens);
  expect(eq(fmt::to_string(ast), expected), location);
}

inline void parseThrows(std::string source,
    const reflection::source_location& location = reflection::source_location::current()) {
  source.append("\n\n");
  const auto tokens = Tokenizer().tokenize(source);
  expect(throws<SyntaxError>([&] {
    Parser().parse(tokens);
  }), location);
}

