#include "astprinter.h"

std::string AstPrinter::print(Stmt& ast)
{
    out.clear();
    indentation = 0;

    walk(ast);

    return fmt::to_string(out);
}

void AstPrinter::before(Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 5, "Update");

    indent();
    fmt::format_to(out, "{}", stmt->type);

    switch (stmt->type)
    {
    case Statement::Type::Block:
        if (stmt->block.identifier.size())
            fmt::format_to(out, " {}", stmt->block.identifier);
        break;
    }

    fmt::format_to(out, "\n");
    indentation++;
}

void AstPrinter::after(Stmt& stmt)
{
    indentation--;
}

void AstPrinter::before(Expr& expr)
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

void AstPrinter::after(Expr& expr)
{
    indentation--;
}

void AstPrinter::indent()
{
    fmt::format_to(out, "{:<{}}", "", 2 * indentation);
}
