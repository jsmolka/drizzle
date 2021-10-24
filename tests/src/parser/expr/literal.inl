static_assert(int(Expression::Literal::Type::LastEnumValue) == 5, "Update");

TEST_CASE("parser_expr_literal")
{
    static constexpr std::string_view kLiterals[] = {
        "true", "false", "1.1", "1", "null", "\"string\"",
    };

    for (const auto& literal : kLiterals)
    {
        const auto source = shell::format("{}\n", literal);

        SECTION(source)
        {
            parse(source, shell::format(R"(program
  expression_statement
    literal {}
)", literal));
        }
    }
}
