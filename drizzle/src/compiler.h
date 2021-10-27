#pragma once

#include "chunk.h"
#include "statement.h"
#include "stringpool.h"

class Compiler
{
public:
    Compiler(StringPool& pool);

    void compile(Stmt& ast, Chunk& chunk);

private:
    void walk(Stmt& stmt);
    void walk(Expr& expr);

    StringPool& pool;
    Chunk* chunk;
};

