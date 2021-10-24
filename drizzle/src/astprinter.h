#pragma once

#include "astwalker.h"
#include "format.h"

class AstPrinter final : public AstWalker
{
public:
    std::string print(Stmt& ast);

protected:
    void before(Stmt& stmt) final;
    void after(Stmt& stmt) final;
    void before(Expr& expr) final;
    void after(Expr& expr) final;

private:
    void indent();

    fmt::memory_buffer out;
    std::size_t indentation;
};
