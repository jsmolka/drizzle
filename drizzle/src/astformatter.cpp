#include "astformatter.h"

std::string AstFormatter::format(Stmt& ast)
{
    out.clear();
    indentation = 0;
    walk(ast);

    return fmt::to_string(out);
}

void AstFormatter::walk(Expr& expr)
{
    static_assert(int(Expression::Type::LastEnumValue) == 6);

    indent();
    fmt::format_to(out, "{}", expr->type);

    switch (expr->type)
    {
    case Expression::Type::Assign:   fmt::format_to(out, " {}", expr->assign.identifier); break;
    case Expression::Type::Binary:   fmt::format_to(out, " {}", expr->binary.type); break;
    case Expression::Type::Literal:  fmt::format_to(out, " {}", expr->literal); break;
    case Expression::Type::Unary:    fmt::format_to(out, " {}", expr->unary.type); break;
    case Expression::Type::Variable: fmt::format_to(out, " {}", expr->variable.identifier); break;
    }

    fmt::format_to(out, "\n");

    indentation++;
    AstWalker::walk(expr);
    indentation--;
}

void AstFormatter::walk(Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 8);

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
    AstWalker::walk(stmt);
    indentation--;
}

void AstFormatter::walk(Statement::If& if_)
{
    walk(if_.if_.condition);
    AstWalker::walk(if_.if_.statements);

    if (if_.elifs.size())
    {
        for (auto& elif : if_.elifs)
        {
            indentation--;
            indent();
            fmt::format_to(out, "elif\n");
            indentation++;

            walk(elif.condition);
            AstWalker::walk(elif.statements);
        }
    }

    if (if_.else_.size())
    {
        indentation--;
        indent();
        fmt::format_to(out, "else\n");
        indentation++;

        AstWalker::walk(if_.else_);
    }
}

void AstFormatter::indent()
{
    fmt::format_to(out, "{:<{}}", "", 2 * indentation);
}
