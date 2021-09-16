#pragma once

#include "value.h"

enum class Opcode
{
    Constant,
    ConstantExt,
    Add,
    Subtract,
    Multiply,
    Divide,
    Not,
    Negate,
    True,
    False,
    Null,
    Return
};

class Chunk
{
public:
    void write(u8 byte);
    void write(Opcode opcode);

    Values constants;
    std::vector<u8> code;
};
