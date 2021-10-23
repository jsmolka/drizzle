TEST_CASE("parser_expr_binary")
{
    static_assert(int(Expression::Binary::Type::LastEnumValue) == 21, "Update test");

    static constexpr std::tuple<Expression::Binary::Type, std::string_view> operations[] =
    {
        { Expression::Binary::Type::And,             "&&"  },
        { Expression::Binary::Type::Addition,        "+"   },
        { Expression::Binary::Type::BitwiseAnd,      "&"   },
        { Expression::Binary::Type::BitwiseAsr,      ">>"  },
        { Expression::Binary::Type::BitwiseLsl,      "<<"  },
        { Expression::Binary::Type::BitwiseLsr,      ">>>" },
        { Expression::Binary::Type::BitwiseOr,       "||"  },
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

    for (const auto& [type, mnemonic] : operations)
    {
        SECTION(std::string(mnemonic))
        {
            const auto source = shell::format("1 {} 1\n", mnemonic);

            parse(source, {
                Statement::Type::Program,
                Statement::Type::ExpressionStatement,
                Expression::Type::Binary,
                Expression::Type::Literal,
                Expression::Type::Literal
            });
        }
    }
}
