#include "compiler.h"

Compiler::Compiler(StringPool& pool)
    : pool(pool)
{
}

void Compiler::compile(Stmt& ast, Chunk& chunk)
{
    this->chunk = &chunk;
}

void Compiler::walk(Stmt& stmt)
{

}

void Compiler::walk(Expr& expr)
{

}
