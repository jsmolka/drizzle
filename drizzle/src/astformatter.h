#pragma once

#include "astwalker.h"
#include "format.h"

class AstFormatter final : public AstWalker
{
public:
    std::string format(Stmt& ast);

protected:
    void walk(Expr& expr) final;
    void walk(Stmt& stmt) final;

private:
    void indent();

    fmt::memory_buffer out;
    std::size_t indentation;
};
