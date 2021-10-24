static_assert(int(Expression::Binary::Type::LastEnumValue) == 21, "Update");

TEST_CASE("parser_expr_binary")
{
    static constexpr std::tuple<Expression::Binary::Type, std::string_view> operations[] =
    {
        { Expression::Binary::Type::And,             "&&"  },
        { Expression::Binary::Type::Addition,        "+"   },
        { Expression::Binary::Type::BitwiseAnd,      "&"   },
        { Expression::Binary::Type::BitwiseAsr,      ">>"  },
        { Expression::Binary::Type::BitwiseLsl,      "<<"  },
        { Expression::Binary::Type::BitwiseLsr,      ">>>" },
        { Expression::Binary::Type::BitwiseOr,       "|"   },
        { Expression::Binary::Type::BitwiseXor,      "^"   },
        { Expression::Binary::Type::Division,        "/"   },
        { Expression::Binary::Type::Equal,           "=="  },
        { Expression::Binary::Type::Greater,         ">"   },
        { Expression::Binary::Type::GreaterEqual,    ">="  },
        { Expression::Binary::Type::IntegerDivision, "//"  },
        { Expression::Binary::Type::Less,            "<"   },
        { Expression::Binary::Type::LessEqual,       "<="  },
        { Expression::Binary::Type::Modulo,          "%"   },
        { Expression::Binary::Type::Multiplication,  "*"   },
        { Expression::Binary::Type::NotEqual,        "!="  },
        { Expression::Binary::Type::Or,              "||"  },
        { Expression::Binary::Type::Power,           "**"  },
        { Expression::Binary::Type::Subtraction,     "-"   },
    };

    for (const auto& [type, operation] : operations)
    {
        const auto source = shell::format("1 {} 1\n", operation);

        SECTION(source)
        {
            parse(source, {
                Statement::Type::Program,
                Statement::Type::ExpressionStatement,
                Expression::Type::Binary,
                Expression::Type::Literal,
                Expression::Type::Literal,
            });

            parseTest<Expr>(source, [type=type](Expr& expr)
            {
                if (expr->type == Expression::Type::Binary)
                    REQUIRE(expr->binary.type == type);
            });
        }
    }
}
