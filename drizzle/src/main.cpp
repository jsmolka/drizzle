#include <shell/filesystem.h>
#include <shell/format.h>
#include <shell/main.h>

#include "error.h"
#include "scanner.h"

std::string source;

std::size_t lineNumber(const char* location)
{
    std::size_t number = 1;
    for (const char* iter = source.data(); iter != location; ++iter)
        number += *iter == '\n';

    return number;
}

std::string_view lineContent(const char* location)
{
    const char* begin = location;
    while (begin > source.data() && begin[-1] != '\n')
        begin--;

    const char* end = location;
    while (*end && *end != '\n')
        end++;

    return std::string_view(begin, end - begin);
}

void syntaxError(const SyntaxError& error)
{
    const auto number = lineNumber(error.location);
    const auto content = lineContent(error.location);
    const auto position = error.location - content.data();
    const auto context = shell::format("[line {}] ", number);
    
    shell::print("{}{}\n", context, content);
    shell::print("{:>{}}\n", "^", position + context.size());
    shell::print("SyntaxError: {}\n", error.what());
}

int main(int argc, char* argv[])
{
    using namespace shell::filesystem;
    try
    {
        if (argc != 2)
        {
            shell::print("Usage: drizzle <file>\n");
            return 0;
        }

        const auto file = u8path(argv[1]);
        const auto status = read(file, source);

        if (status != Status::Ok)
        {
            shell::print("Cannot read file \"{}\"\n", file);
            return 1;
        }

        Scanner scanner;
        const auto tokens = scanner.scan(source);

        return 0;
    }
    catch (const SyntaxError& error)
    {
        syntaxError(error);
        return 1;
    }
    catch (const std::exception& error)
    {
        shell::print("Error: {}\n", error.what());
        return 1;
    }
}
