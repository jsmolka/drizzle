#include <string>
#include <shell/filesystem.h>
#include <shell/format.h>
#include <shell/main.h>

#include "error.h"
#include "scanner.h"
#include "vm.h"

std::string source;

std::size_t line(const char* location)
{
    std::size_t line = 1;
    for (const auto& c : source)
    {
        if (location == &c)
            break;

        line += c == '\n';
    }
    return line;
}

std::string_view context(const char* location)
{
    const char* beg = location;
    const char* end = location;

    while (beg > source.data())
    {
        if (beg[-1] == '\n')
            break;
        else
            beg--;
    }

    while (end < source.data() + source.size())
    {
        if (end[1] == '\n')
            break;
        else
            end++;
    }

    return std::string_view(beg, end - beg);
}

void syntaxError(const SyntaxError& error)
{
    const auto line = shell::format("[line {}] ", ::line(error.location));
    const auto context = ::context(error.location);
    const auto position = error.location - context.data();

    shell::print("{}{}\n", line, context);
    shell::print("{:>{}}\n", "^", position + line.size());
    shell::print("Syntax Error: {}\n", error.what());
}

int main(int argc, char* argv[])
{
    namespace fs = shell::filesystem;

    if (argc != 2)
    {
        shell::print("Usage: drizzle <file>\n");
        return 0;
    }

    try
    {
        const auto file = fs::u8path(argv[1]);
        if (fs::read(file, source) != fs::Status::Ok)
        {
            shell::print("Cannot read file '{}'", file);
            return 1;
        }

        Scanner scanner;
        const auto tokens = scanner.scan(source);

        Vm vm;
        vm.interpret(std::string_view(source));

        return 0;
    }
    catch (const SyntaxError& error)
    {
        syntaxError(error);
        return 1;
    }
    catch (const std::exception& error)
    {
        shell::print("{}\n", error.what());
        return 1;
    }
}
