#include "astprinter.h"

std::string AstPrinter::print(Stmt& ast)
{
    out = fmt::memory_buffer();
    indentation = 0;

    walk(ast);

    return fmt::to_string(out);
}

void AstPrinter::before(Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 3, "Update");

    indent();
    fmt::format_to(out, "{}\n", stmt->type);
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
    fmt::format_to(out, "{:^{}}", "", kSpaces * indentation);
}
