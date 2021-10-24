static_assert(int(Expression::Unary::Type::LastEnumValue) == 3, "Update");

TEST_CASE("parser_expr_precedence_1")
{
    static constexpr std::string_view operations[] = { "-", "!", "~" };

    for (const auto& operation : operations)
    {
        auto source = shell::format("{}1 * 1\n", operation);

        SECTION(source)
        {
            parse(source, {
                Statement::Type::Program,
                Statement::Type::ExpressionStatement,
                Expression::Type::Binary,
                Expression::Type::Unary,
                Expression::Type::Literal,
                Expression::Type::Literal,
            });
        }

        source = shell::format("{}(1 * 1)\n", operation);

        SECTION(source)
        {
            parse(source, {
                Statement::Type::Program,
                Statement::Type::ExpressionStatement,
                Expression::Type::Unary,
                Expression::Type::Binary,
                Expression::Type::Literal,
                Expression::Type::Literal,
            });
        }
    }
}

static_assert(int(Expression::Binary::Type::LastEnumValue) == 21, "Update");

TEST_CASE("parser_expr_precedence_2")
{
    std::vector<std::vector<std::string_view>> precedences = {
        { "*", "/", "%", "//", "**" },
        { "+", "-" },
        { "<<", ">>", ">>>" },
        { "<", "<=", ">", ">=" },
        { "==", "!=" },
        { "&" },
        { "^" },
        { "|" },
        { "&&" },
        { "||" },
    };

    for (int i = 0; i < precedences.size() - 1; ++i)
    {
        const auto& highs = precedences[i];
        const auto& lows  = precedences[i + 1];

        for (const auto& high : highs)
        {
            for (const auto& low : lows)
            {
                auto source = shell::format("1 {} 1 {} 1\n", low, high);

                SECTION(source)
                {
                    parse(source, {
                        Statement::Type::Program,
                        Statement::Type::ExpressionStatement,
                        Expression::Type::Binary,
                        Expression::Type::Literal,
                        Expression::Type::Binary,
                        Expression::Type::Literal,
                        Expression::Type::Literal,
                    });
                }

                source = shell::format("(1 {} 1) {} 1\n", low, high);

                SECTION(source)
                {
                    parse(source, {
                        Statement::Type::Program,
                        Statement::Type::ExpressionStatement,
                        Expression::Type::Binary,
                        Expression::Type::Binary,
                        Expression::Type::Literal,
                        Expression::Type::Literal,
                        Expression::Type::Literal,
                    });
                }
            }
        }
    }
}
