TEST_CASE("scanner::indentation1")
{
    using Type = Token::Type;

    constexpr std::string_view kContent = R"(
|
  |
    |
  |
|)";

    //require(kContent, {
    //    Type::Pipe,
    //    Type::NewLine,
    //    Type::Indent,
    //    Type::Pipe,
    //    Type::NewLine,
    //    Type::Indent,
    //    Type::Pipe,
    //    Type::NewLine,
    //    Type::Dedent,
    //    Type::Pipe,
    //    Type::NewLine,
    //    Type::Dedent,
    //    Type::Pipe,
    //    Type::Eof
    //});

    REQUIRE(true);
}
