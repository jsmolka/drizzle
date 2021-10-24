static_assert(int(Expression::Unary::Type::LastEnumValue) == 3, "Update");

TEST_CASE("parser_expr_precedence_1")
{
    static constexpr std::string_view kOperations[] = { "-", "!", "~" };

    for (const auto& operation : kOperations)
    {
        auto source = shell::format("{}1 * 2\n", operation);

        SECTION(source)
        {
            parse(source, shell::format(R"(program
  expression_statement
    binary *
      unary {}
        literal 1
      literal 2
)", operation));
        }

        source = shell::format("{}(1 * 2)\n", operation);

        SECTION(source)
        {
            parse(source, shell::format(R"(program
  expression_statement
    unary {}
      binary *
        literal 1
        literal 2
)", operation));
        }
    }
}

static_assert(int(Expression::Binary::Type::LastEnumValue) == 21, "Update");

TEST_CASE("parser_expr_precedence_2")
{
    std::vector<std::vector<std::string_view>> precedences =
    {
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

    for (std::size_t i = 0; i < precedences.size() - 1; ++i)
    {
        const auto& highs = precedences[i];
        const auto& lows  = precedences[i + 1];

        for (const auto& high : highs)
        {
            for (const auto& low : lows)
            {
                auto source = shell::format("1 {} 2 {} 3\n", low, high);

                SECTION(source)
                {
                    parse(source, shell::format(R"(program
  expression_statement
    binary {}
      literal 1
      binary {}
        literal 2
        literal 3
)", low, high));
                }

                source = shell::format("(1 {} 2) {} 3\n", low, high);

                SECTION(source)
                {
                    parse(source, shell::format(R"(program
  expression_statement
    binary {}
      binary {}
        literal 1
        literal 2
      literal 3
)", high, low));
                }
            }
        }
    }
}
