#pragma once

#include "astformatter.h"
#include "compiler.h"
#include "error.h"
#include "parser.h"
#include "tokenizer.h"
#include "ut.h"

namespace {

inline auto tokenizeThrowsNot(const std::string& source,
    const reflection::source_location& location) -> std::vector<Token> {
  std::vector<Token> tokens;
  expect((!throws([&] {
    tokens = Tokenizer().tokenize(source);
  }) >> fatal), location) << "tokenize" << source;
  return tokens;
}

inline auto parseThrowsNot(const std::string& source,
    const reflection::source_location& location) -> Stmt {
  const auto tokens = tokenizeThrowsNot(source, location);
  Stmt ast;
  expect((!throws([&] {
    ast = Parser().parse(tokens);
  }) >> fatal), location) << "parse" << source;
  return ast;
}

}  // namespace

inline auto operator<<(std::ostream& out, const Token::Type& type) -> std::ostream& {
  return out << fmt::to_string(type);
}

inline void tokenize(const std::string& source, const std::vector<Token::Type>& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = tokenizeThrowsNot(source, location);
  expect(eq(tokens.size(), expected.size()), location) << source;
  for (std::size_t i = 0; i < std::min(tokens.size(), expected.size()); ++i) {
    expect(eq(tokens[i].type, expected[i]), location) << source;
  }
}

inline void tokenizeThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  expect(throws<SyntaxError>([&] {
    Tokenizer().tokenize(source);
  }), location) << "tokenize" << source;
}

inline void parse(const std::string& source, const std::string& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto ast = parseThrowsNot(source, location);
  expect(eq(fmt::to_string(ast), expected), location) << source;
}

inline void parseThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = tokenizeThrowsNot(source, location);
  expect(throws<SyntaxError>([&] {
    Parser().parse(tokens);
  }), location) << "parse" << source;
}

inline void compileThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto ast = parseThrowsNot(source, location);
  expect(throws<SyntaxError>([&] {
    Compiler().compile(ast);
  }), location) << "compile" << source;
}
