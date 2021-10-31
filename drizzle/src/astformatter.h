#pragma once

#include "astwalker.h"
#include "format.h"

class AstFormatter final : public AstWalker
{
public:
    std::string format(Stmt& ast);

protected:
    using AstWalker::walk;

    void walk(Expr& expr) final;
    void walk(Stmt& stmt) final;
    void walk(Statement::If& if_) final;

private:
    void indent();
    void identifier(std::string_view identifier);

    fmt::memory_buffer out;
    std::size_t indentation;
};
