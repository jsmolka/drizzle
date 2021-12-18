#include <sh/argparse.h>
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
  using namespace sh;
  using namespace sh::filesystem;

  sh::argument_parser parser("drizzle");
  parser.add<std::optional<bool>>("-h", "--help") | sh::description("print help");
  parser.add<std::optional<bool>>("-a", "--ast") | sh::description("print AST");
  parser.add<path>("file") | sh::description("file to execute");

  try {
    parser.parse(argc, argv);
  } catch (const std::runtime_error& error) {
    fmt::print("{}\n\n{}\n", error.what(), parser.help());
    return 1;
  }

  const auto help = parser.get<std::optional<bool>>("-h");
  if (help && *help) {
    fmt::print("{}\n", parser.help());
    return 0;
  }

  try {
    const auto file = parser.get<path>("file");
    const auto print_ast = parser.get<std::optional<bool>>("-a");

    const auto status = read(file, source);
    if (status != filesystem::status::ok) {
      fmt::print("cannot read file '{}'\n", file);
      return 1;
    }

    source.push_back('\n');
    source.push_back('\n');

    const auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);

    if (print_ast && *print_ast) {
      fmt::print("{}", AstFormatter().format(ast));
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
