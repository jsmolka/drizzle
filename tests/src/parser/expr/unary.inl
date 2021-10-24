static_assert(int(Expression::Unary::Type::LastEnumValue) == 3, "Update");

TEST_CASE("parser_expr_unary")
{
    static constexpr std::tuple<Expression::Unary::Type, std::string_view> operations[] =
    {
        { Expression::Unary::Type::BitwiseComplement, "~" },
        { Expression::Unary::Type::Minus,             "-" },
        { Expression::Unary::Type::Not,               "!" },
    };

    for (const auto& [type, operation] : operations)
    {
        const auto source = shell::format("{}1\n", operation);

        SECTION(source)
        {
            parse(source, {
                Statement::Type::Program,
                Statement::Type::ExpressionStatement,
                Expression::Type::Unary,
                Expression::Type::Literal,
            });

            parseTest<Expr>(source, [type=type](Expr& expr)
            {
                if (expr->type == Expression::Type::Unary)
                    REQUIRE(expr->unary.type == type);
            });
        }
    }
}
