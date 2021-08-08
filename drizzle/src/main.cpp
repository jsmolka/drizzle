#define FMT_HEADER_ONLY
#include <fmt/core.h>

#ifdef _MSC_VER

#include <codecvt>
#include <cstring>
#include <vector>

int main(int argc, char* argv[]);

#pragma comment(linker, "/ENTRY:wmainCRTStartup")

int wmain(int argc, wchar_t* argv[])
{
    using Convert = std::wstring_convert<std::codecvt_utf8<wchar_t>>;

    std::vector<char*> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i)
    {
        auto string = Convert().to_bytes(argv[i]);
        args.push_back(strdup(string.c_str()));
    }

    int ret = main(argc, args.data());

    for (auto arg : args)
        free(arg);

    return ret;
}

#endif

int main(int argc, char* argv[])
{
    try
    {
        return 0;
    } 
    catch (const std::exception& ex)
    {
        fmt::print(ex.what());
        return 1;
    }
}
