#include <string>
#include <string_view>
#include <shell/filesystem.h>
#include <shell/format.h>
#include <shell/main.h>

#include "error.h"
#include "vm.h"

int main(int argc, char* argv[])
{
    namespace filesystem = shell::filesystem;

    if (argc < 2)
    {
        shell::print("Usage: drizzle <file>\n");
        return 0;
    }

    try
    {
        auto file = filesystem::u8path(argv[1]);
        auto [status, source] = filesystem::read<std::string>(file);

        if (status != filesystem::Status::Ok)
        {
            shell::print("Could not read file '{}'\n", file);
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
