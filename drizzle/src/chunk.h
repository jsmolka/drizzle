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
    void write(u8 byte, std::size_t line);
    void write(Opcode opcode, std::size_t line);

    Values constants;
    std::vector<u8> code;
    std::vector<std::size_t> lines;
};
