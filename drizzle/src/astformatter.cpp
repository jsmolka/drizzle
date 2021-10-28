#include "astformatter.h"

std::string AstFormatter::format(Stmt& ast)
{
    out.clear();
    indentation = 0;

    walk(ast);

    return fmt::to_string(out);
}

void AstFormatter::before(Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 6, "Update");

    indent();
    fmt::format_to(out, "{}", stmt->type);

    switch (stmt->type)
    {
    case Statement::Type::Block:
        if (stmt->block.identifier.size())
            fmt::format_to(out, " {}", stmt->block.identifier);
        break;

    case Statement::Type::Var:
        fmt::format_to(out, " {}", stmt->var.identifier);
        break;
    }

    fmt::format_to(out, "\n");
    indentation++;
}

void AstFormatter::after(Stmt& stmt)
{
    indentation--;
}

void AstFormatter::before(Expr& expr)
{
    static_assert(int(Expression::Type::LastEnumValue) == 4, "Update");

    indent();
    fmt::format_to(out, "{}", expr->type);

    switch (expr->type)
    {
    case Expression::Type::Binary:
        fmt::format_to(out, " {}", expr->binary.type);
        break;

    case Expression::Type::Literal:
        fmt::format_to(out, " {}", expr->literal);
        break;

    case Expression::Type::Unary:
        fmt::format_to(out, " {}", expr->unary.type);
        break;
    }

    fmt::format_to(out, "\n");
    indentation++;
}

void AstFormatter::after(Expr& expr)
{
    indentation--;
}

void AstFormatter::indent()
{
    fmt::format_to(out, "{:<{}}", "", 2 * indentation);
}
