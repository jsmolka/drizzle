static_assert(int(Expression::Literal::Type::LastEnumValue) == 5, "Update");

TEST_CASE("parser_expr_literal")
{
    static constexpr std::tuple<Expression::Literal::Type, std::string_view> literals[] =
    {
        { Expression::Literal::Type::Boolean, "true"     },
        { Expression::Literal::Type::Boolean, "false"    },
        { Expression::Literal::Type::Float,   "1.1"      },
        { Expression::Literal::Type::Integer, "1"        },
        { Expression::Literal::Type::Null,    "null"     },
        { Expression::Literal::Type::String,  "\"test\"" },
    };

    for (const auto& [type, literal] : literals)
    {
        const auto source = shell::format("{}\n", literal);

        SECTION(source)
        {
            parse(source, {
                Statement::Type::Program,
                Statement::Type::ExpressionStatement,
                Expression::Type::Literal,
            });

            parseTest<Expr>(source, [type=type](Expr& expr)
            {
                if (expr->type == Expression::Type::Literal)
                    REQUIRE(expr->literal.type == type);
            });
        }
    }
}
