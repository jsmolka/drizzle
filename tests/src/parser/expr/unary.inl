static_assert(int(Expression::Unary::Type::LastEnumValue) == 3, "Update");

TEST_CASE("parser_expr_unary")
{
    static constexpr std::string_view kOperations[] = { "~", "-", "!", };

    for (const auto& operation : kOperations)
    {
        const auto source = shell::format("{}1\n", operation);

        SECTION(source)
        {
            parse(source, shell::format(R"(program
  expression_statement
    unary {}
      literal 1
)", operation));
        }
    }
}
