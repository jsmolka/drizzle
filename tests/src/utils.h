#pragma once

#include "astformatter.h"
#include "compiler.h"
#include "error.h"
#include "gc.h"
#include "parser.h"
#include "tokenizer.h"
#include "ut.h"
#include "vm.h"

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
  Stmt ast;
  const auto tokens = tokenizeThrowsNot(source, location);
  expect((!throws([&] {
    ast = Parser().parse(tokens);
  }) >> fatal), location) << "parse" << source;
  return ast;
}

inline auto compileThrowsNot(const std::string& source, Gc& gc,
    const reflection::source_location& location) -> Program {
  Program program;
  const auto ast = parseThrowsNot(source, location);
  expect((!throws([&] {
    program = Compiler(gc).compile(ast);
  }) >> fatal), location) << "compile" << source;
  return program;
}

}  // namespace

inline auto operator<<(std::ostream& out, const Token::Type& type) -> std::ostream& {
  return out << fmt::to_string(type);
}

inline void tokenize(const std::string& source, const std::vector<Token::Type>& expected,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto tokens = tokenizeThrowsNot(source, location);
  expect(eq(tokens.size(), expected.size()), location) << source;
  for (auto i = 0; i < std::min(tokens.size(), expected.size()); ++i) {
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

template<typename Error>
inline void compileThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  const auto ast = parseThrowsNot(source, location);
  expect(throws<Error>([&] {
    Gc gc;
    Compiler(gc).compile(ast);
  }), location) << "compile" << source;
}

inline void run(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  Gc gc;
  const auto program = compileThrowsNot(source, gc, location);
  expect(!throws([&] {
    Vm(gc).interpret(program);
  }), location) << "run" << source;
}

inline void runThrows(const std::string& source,
    const reflection::source_location& location = reflection::source_location::current()) {
  Gc gc;
  const auto program = compileThrowsNot(source, gc, location);
  expect(throws<RuntimeError>([&] {
    Vm(gc).interpret(program);
  }), location) << "run" << source;
}
