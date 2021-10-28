#include <shell/filesystem.h>
#include <shell/main.h>
#include <shell/options.h>

#include "astprinter.h"
#include "compiler.h"
#include "errors.h"
#include "parser.h"
#include "tokenizer.h"
#include "vm.h"

std::string source;

std::size_t sourceLine(const char* location)
{
    if (location < source.data() || location >= source.data() + source.size())
        return -1;

    std::size_t line = 0;
    for (const char* c = source.data(); c != location; ++c)
        line += *c == '\n';

    return line;
}

std::string_view sourceView(std::size_t line)
{
    const char* b = source.data();
    const char* c = source.data();

    for (; *c; ++c)
    {
        if (!(*c == '\n' || *c == '\0'))
            continue;

        if (line--)
            b = c + 1;
        else
            return std::string_view(b, c - b);
    }
    return "<error>";
}

void printErrorLine(std::size_t line)
{
    const auto view = sourceView(line);
    const auto info = shell::format("Line {} | ", line + 1);

    shell::print("{}{}\n\n", info, view);
}

void printErrorLocation(const char* location)
{
    const auto line = sourceLine(location);
    const auto view = sourceView(line);
    const auto info = shell::format("Line {} | ", line + 1);

    std::string whitespace(info.size(), ' ');
    for (const auto& c : view)
    {
        if (&c == location)
            break;

        if (c == '\t')
            whitespace.push_back('\t');
        if (std::isprint(c))
            whitespace.push_back(' ');
    }

    shell::print("{}{}\n", info, view);
    shell::print("{}^\n", whitespace);
}

void printError(const Error& error)
{
    if (error.location.type == Error::Location::Type::Line)
        printErrorLine(error.location.line);
    if (error.location.type == Error::Location::Type::Location)
        printErrorLocation(error.location.location);

    shell::print("{}: {}\n", error.name(), error.what());
}

int main(int argc, char* argv[])
{
    using namespace shell;
    using namespace shell::filesystem;

    Options options("drizzle");
    options.add({ "-a,--ast", "print ast"   }, Options::value<bool>(false));
    options.add({     "file", "script file" }, Options::value<filesystem::path>()->positional());

    try
    {
        const auto result = options.parse(argc, argv);
        const auto file(*result.find<filesystem::path>("file"));
        const auto print_ast(*result.find<bool>("--ast"));

        const auto status = read(file, source);

        if (status != Status::Ok)
        {
            shell::print("Cannot read file '{}'\n", file);
            return 1;
        }

        source.push_back('\n');
        source.push_back('\n');

        const auto tokens = Tokenizer().tokenize(source);
        auto ast = Parser().parse(tokens);

        if (print_ast)
        {
            fmt::print("{}\n", AstPrinter().print(ast));
        }
        else
        {
            StringPool pool;
            Chunk chunk;
            Compiler(pool).compile(ast, chunk);
            Vm(pool).interpret(chunk);
        }
        return 0;
    }
    catch (const ::Error& error)
    {
        printError(error);
        return 1;
    }
    catch (const shell::ParseError& error)
    {
        shell::print("OptionsError: {}\n", error.what());
        return 1;
    }
    catch (const std::exception& error)
    {
        shell::print("Unknown error: {}\n", error.what());
        return 1;
    }
}
