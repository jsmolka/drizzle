#pragma once

#include "astformatter.h"
#include "errors.h"
#include "parser.h"
#include "tokenizer.h"
#include "ut.h"

inline auto operator<<(std::ostream& out, const Token::Type& type) -> std::ostream& {
  return out << fmt::to_string(type);
}

inline void tokenize(const std::string& source, const std::vector<Token::Type>& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  std::vector<Token> tokens;
  expect(!throws([&] {
    tokens = Tokenizer().tokenize(source);
  }), location) << source;
  expect(eq(tokens.size(), expected.size()), location) << source;
  for (std::size_t i = 0; i < std::min(tokens.size(), expected.size()); ++i) {
    expect(eq(tokens[i].type, expected[i]), location) << source;
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
  Stmt ast;
  expect(!throws([&] {
    const auto tokens = Tokenizer().tokenize(source);
    ast = Parser().parse(tokens);
  }), location) << source;
  expect(eq(fmt::to_string(ast), expected), location) << source;
}

inline void parseThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  expect(throws<SyntaxError>([&] {
    const auto tokens = Tokenizer().tokenize(source);
    Parser().parse(tokens);
  }), location) << source;
}
