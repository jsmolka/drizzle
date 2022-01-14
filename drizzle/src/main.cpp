#include <sh/clap.h>
#include <sh/filesystem.h>
#include <sh/main.h>

#include "astformatter.h"
#include "compiler.h"
#include "errors.h"
#include "parser.h"
#include "tokenizer.h"
#include "vm.h"

auto sourceLine(const std::string& source, std::size_t line) -> std::string {
  std::stringstream stream(source);
  std::string data;
  do {
    std::getline(stream, data);
  } while (line--);
  return data;
}

void report(const Error& error, const std::string& source) {
  const auto line = sourceLine(source, error.location.line);
  const auto info = fmt::format("Line {} | ", error.location.line + 1);
  fmt::print("{}{}\n", info, line);

  if (error.location.column) {
    std::string whitespace(info.size(), ' ');
    for (const auto [i, c] : sh::enumerate(line)) {
      if (i == *error.location.column) {
        break;
      } else if (c == '\t') {
        whitespace.push_back('\t');
      } else if (std::isprint(c)) {
        whitespace.push_back(' ');
      }
    }
    fmt::print("{}^\n", whitespace);
  }
  fmt::print("{}: {}\n", error.name(), error.what());
}

auto main(int argc, char* argv[]) -> int {
  std::filesystem::path file;
  std::optional<bool> print = false;

  sh::clap parser("drizzle");
  parser.add_help();
  parser.add<decltype(print)>("-a", "--ast") << &print << sh::desc("print ast");
  parser.add<decltype(file)>("file") << &file << sh::desc("script file");
  parser.try_parse(argc, argv);

  std::string source;
  try {
    if (sh::filesystem::read(file, source) != sh::filesystem::status::ok) {
      fmt::print("cannot read file: {}\n", file);
      return 1;
    }

    auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);

    if (*print) {
      fmt::print("{}\n", ast);
    } else {
      Chunk chunk;
      StringPool pool;
      Compiler(pool).compile(ast, chunk);
      Vm(pool).interpret(chunk);
    }
    return 0;
  } catch (const Error& error) {
    report(error, source);
    return 1;
  } catch (const std::exception& error) {
    fmt::print("unknown error: {}\n", error.what());
    return 1;
  }
}
