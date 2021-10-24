static_assert(int(Expression::Binary::Type::LastEnumValue) == 21, "Update");

TEST_CASE("parser_expr_binary")
{
    static constexpr std::string_view kOperations[] = {
        "&&", "+", "&", ">>", "<<", ">>>", "|", "^", "/", "==", ">", ">=", "//", "<", "<=", "%", "*", "!=", "||", "**", "-",
    };

    for (const auto& operation : kOperations)
    {
        const auto source = shell::format("1 {} 2\n", operation);

        SECTION(source)
        {
            parse(source, shell::format(R"(program
  expression_statement
    binary {}
      literal 1
      literal 2
)", operation));
        }
    }
}
