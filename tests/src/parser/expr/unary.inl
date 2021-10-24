TEST_CASE("parser_expr_unary")
{
    for (int i = 0; i < int(Expression::Unary::Type::LastEnumValue); ++i)
    {
        const auto type = Expression::Unary::Type(i);
        const auto source = shell::format("{}1\n", type);

        SECTION(source)
        {
            parse(source, shell::format(R"(program
  expression_statement
    unary {}
      literal 1
)", type));
        }
    }
}
