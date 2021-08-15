#include <string>
#include <string_view>
#include <shell/filesystem.h>
#include <shell/format.h>
#include <shell/main.h>

#include "error.h"
#include "vm.h"

std::string source;

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
        auto file = fs::u8path(argv[1]);
        if (fs::read(file, source) != fs::Status::Ok)
        {
            shell::print("Cannot read file '{}'", file);
            return 1;
        }

        Vm vm;
        vm.interpret(std::string_view(source));

        return 0;
    }
    catch (const SyntaxError& error)
    {
        shell::print("Syntax Error: {}\n", error.what());
        return 1;
    }
    catch (const std::exception& error)
    {
        shell::print("{}\n", error.what());
        return 1;
    }
}
