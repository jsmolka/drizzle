#include "astformatter.h"

std::string AstFormatter::format(Stmt& ast)
{
    buffer.clear();
    indentation = 0;
    walk(ast);

    return fmt::to_string(buffer);
}

void AstFormatter::walk(Expr& expr)
{
    static_assert(int(Expression::Type::LastEnumValue) == 6);

    writeIndent("{}", expr->type);

    switch (expr->type)
    {
    case Expression::Type::Assign:   write(" {}", expr->assign.identifier); break;
    case Expression::Type::Binary:   write(" {}", expr->binary.type); break;
    case Expression::Type::Literal:  write(" {}", expr->literal); break;
    case Expression::Type::Unary:    write(" {}", expr->unary.type); break;
    case Expression::Type::Variable: write(" {}", expr->variable.identifier); break;
    }

    write("\n");

    indentation++;
    AstWalker::walk(expr);
    indentation--;
}

void AstFormatter::walk(Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 10);

    writeIndent("{}", stmt->type);

    switch (stmt->type)
    {
    case Statement::Type::Block: write(" {}", stmt->block.identifier); break;
    case Statement::Type::Break: write(" {}", stmt->break_.identifier); break;
    case Statement::Type::Var:   write(" {}", stmt->var.identifier); break;
    }

    write("\n");

    indentation++;
    AstWalker::walk(stmt);
    indentation--;
}

void AstFormatter::walk(Statement::If& if_)
{
    walk(if_.if_.condition);
    walk(if_.if_.statements);

    for (auto& elif : if_.elifs)
    {
        indentation--;
        writeIndent("elif\n");
        indentation++;

        walk(elif.condition);
        walk(elif.statements);
    }

    if (if_.else_.size())
    {
        indentation--;
        writeIndent("else\n");
        indentation++;

        walk(if_.else_);
    }
}

template<typename T>
void AstFormatter::write(std::string_view format, const T& value)
{
    if constexpr (std::is_same_v<T, std::string_view>)
    {
        if (value.empty())
            return;
    }
    fmt::format_to(buffer, format, value);
}

void AstFormatter::write(std::string_view format)
{
    write("{}", format);
}

template<typename T>
void AstFormatter::writeIndent(std::string_view format, const T& value)
{
    if constexpr (std::is_same_v<T, std::string_view>)
    {
        if (value.empty())
            return;
    }
    fmt::format_to(buffer, "{:<{}}", "", 2 * indentation);
    fmt::format_to(buffer, format, value);
}

void AstFormatter::writeIndent(std::string_view format)
{
    writeIndent("{}", format);
}
