#pragma once

#include "int.h"
#include "value.h"

enum class Opcode
{
    Return,
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
    Equal,
    NotEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    Modulo
};

class Chunk
{
public:
    void write(u8 byte);
    void write(Opcode opcode);

    Values constants;
    std::vector<u8> code;
};
