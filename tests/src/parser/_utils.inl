void parse(const std::string& source, const std::string& expect)
{
    const auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);
    REQUIRE(AstPrinter().print(ast) == expect);
}
