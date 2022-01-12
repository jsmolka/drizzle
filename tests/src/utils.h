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

inline void tokenizeThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  expect(throws<SyntaxError>([&] {
    Tokenizer().tokenize(source);
  }), location) << source;
}

inline void parse(const std::string& source, const std::string& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = Tokenizer().tokenize(source);
  const auto ast = Parser().parse(tokens);
  expect(eq(fmt::to_string(ast), expected), location);
}

inline void parseThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  expect(throws<SyntaxError>([&] {
    const auto tokens = Tokenizer().tokenize(source);
    Parser().parse(tokens);
  }), location) << source;
}

