#include <shell/filesystem.h>
#include <shell/format.h>
#include <shell/main.h>

#include "errors.h"

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
    static constexpr std::string_view kDummy = "dummy";

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
    return kDummy;
}

void locationError(const LocationError& error)
{
    const auto line = sourceLine(error.location);
    const auto view = sourceView(line);
    const auto info = shell::format("Line {} | ", line + 1);

    std::string whitespace(info.size(), ' ');
    for (const auto& c : view)
    {
        if (&c == error.location)
            break;

        if (c == '\t')
            whitespace.push_back('\t');
        if (std::isprint(c))
            whitespace.push_back(' ');
    }

    shell::print("{}{}\n", info, view);
    shell::print("{}^\n", whitespace);
    shell::print("{}: {}\n", error.name(), error.what());
}

void lineError(const LineError& error)
{
    const auto view = sourceView(error.line);
    const auto info = shell::format("Line {} | ", error.line + 1);

    shell::print("{}{}\n\n", info, view);
    shell::print("{}: {}\n", error.name(), error.what());
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
            shell::print("Cannot read file '{}'\n", file);
            return 1;
        }

        source.push_back('\n');
        source.push_back('\n');

        return 0;
    }
    catch (const LocationError& error)
    {
        locationError(error);
        return 1;
    }
    catch (const LineError& error)
    {
        lineError(error);
        return 1;
    }
    catch (const Error& error)
    {
        shell::print("{}: {}\n", error.name(), error.what());
        return 1;
    }
    catch (const std::exception& error)
    {
        shell::print("Unknown: {}\n", error.what());
        return 1;
    }
}
