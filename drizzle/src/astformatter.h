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
    template<typename T>
    void write(std::string_view format, const T& value);
    void write(std::string_view format);

    template<typename T>
    void writeIndent(std::string_view format, const T& value);
    void writeIndent(std::string_view format);

    fmt::memory_buffer buffer;
    std::size_t indentation;
};
