#include <sh/clap.h>
#include <sh/filesystem.h>
#include <sh/main.h>

#include "astformatter.h"
#include "compiler.h"
#include "errors.h"
#include "parser.h"
#include "tokenizer.h"
#include "vm.h"

std::string source;

std::size_t sourceLine(const char* location) {
  if (location < source.data() || location >= source.data() + source.size()) return -1;

  std::size_t line = 0;
  for (const char* c = source.data(); c != location; ++c)
    line += *c == '\n';

  return line;
}

std::string_view sourceView(std::size_t line) {
  const char* b = source.data();
  const char* c = source.data();

  for (; *c; ++c) {
    if (!(*c == '\n' || *c == '\0')) continue;

    if (line--)
      b = c + 1;
    else
      return std::string_view(b, c - b);
  }
  return "<error>";
}

void printErrorLine(std::size_t line) {
  const auto view = sourceView(line);
  const auto info = fmt::format("Line {} | ", line + 1);

  fmt::print("{}{}\n\n", info, view);
}

void printErrorLocation(const char* location) {
  const auto line = sourceLine(location);
  const auto view = sourceView(line);
  const auto info = fmt::format("Line {} | ", line + 1);

  std::string whitespace(info.size(), ' ');
  for (const auto& c : view) {
    if (&c == location) break;

    if (c == '\t') whitespace.push_back('\t');
    if (std::isprint(c)) whitespace.push_back(' ');
  }

  fmt::print("{}{}\n", info, view);
  fmt::print("{}^\n", whitespace);
}

void printError(const Error& error) {
  if (error.location.type == Error::Location::Type::Line) printErrorLine(error.location.line);
  if (error.location.type == Error::Location::Type::Location)
    printErrorLocation(error.location.location);

  fmt::print("{}: {}\n", error.name(), error.what());
}

int main(int argc, char* argv[]) {
  std::optional<bool> print = false;
  std::filesystem::path file;

  sh::clap parser("drizzle");
  parser.add_help();
  parser.add<decltype(print)>("-a", "--ast") << &print << sh::desc("print ast");
  parser.add<decltype(file)>("file") << &file << sh::desc("script file");
  parser.try_parse(argc, argv);

  try {
    const auto status = sh::filesystem::read(file, source);
    if (status != sh::filesystem::status::ok) {
      fmt::print("cannot read file: {}\n", file);
      return 0;
    }

    source.push_back('\n');
    source.push_back('\n');

    const auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);

    if (*print) {
      fmt::print("{}", ast);
    } else {
      StringPool pool;
      Chunk chunk;
      Compiler(pool).compile(ast, chunk);
      Vm(pool).interpret(chunk);
    }
    return 0;
  } catch (const Error& error) {
    printError(error);
    return 1;
  } catch (const std::exception& error) {
    fmt::print("unknown error: {}\n", error.what());
    return 1;
  }
}
