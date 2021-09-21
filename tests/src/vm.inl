void vmExec(std::string_view source)
{
    std::string code(source);
    code.push_back('\n');
    
    Scanner scanner;
    const auto tokens = scanner.scan(code);

    Vm vm;
    try
    {
        vm.interpret(tokens);
        REQUIRE(true);
    }
    catch (const std::exception& ex)
    {
        REQUIRE(false);
    }
}

template<typename Error>
void vmThrows(std::string_view source)
{
    static_assert(std::is_base_of_v<RuntimeError, Error>);

    std::string code(source);
    code.push_back('\n');

    Scanner scanner;
    const auto tokens = scanner.scan(code);

    Vm vm;
    REQUIRE_THROWS_AS(vm.interpret(tokens), Error);
}

#include "vm/assertion.inl"
