TEST_CASE("parser_expr_group_1")
{
    constexpr auto kSource = "(1 * 1) + (1 * 1)\n";

    parse(kSource, {
        Statement::Type::Program,
        Statement::Type::ExpressionStatement,
        Expression::Type::Binary,
        Expression::Type::Binary,
        Expression::Type::Literal,
        Expression::Type::Literal,
        Expression::Type::Binary,
        Expression::Type::Literal,
        Expression::Type::Literal,
    });
}

TEST_CASE("parser_expr_group_2")
{
    //constexpr auto kSource = "1 * (2 + 3) * 4\n";

    //parse(kSource, {
    //    Statement::Type::Program,
    //    Statement::Type::ExpressionStatement,
    //    Expression::Type::Binary,
    //    Expression::Type::Literal,
    //    Expression::Type::Binary,
    //    Expression::Type::Binary,
    //    Expression::Type::Literal,
    //    Expression::Type::Literal,
    //    Expression::Type::Literal,
    //});
}
