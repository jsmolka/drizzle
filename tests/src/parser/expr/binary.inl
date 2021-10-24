TEST_CASE("parser_expr_binary")
{
    for (int i = 0; i < int(Expression::Binary::Type::LastEnumValue); ++i)
    {
        const auto type = Expression::Binary::Type(i);
        const auto source = shell::format("1 {} 2\n", type);

        SECTION(source)
        {
            parse(source, shell::format(R"(program
  expression_statement
    binary {}
      literal 1
      literal 2
)", type));
        }
    }
}
