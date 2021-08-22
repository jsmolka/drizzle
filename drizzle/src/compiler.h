#pragma once

#include "chunk.h"
#include "token.h"

class Compiler
{
public:
    void compile(const Tokens& tokens, Chunk& chunk);
};
