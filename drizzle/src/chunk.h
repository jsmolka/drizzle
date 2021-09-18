#pragma once

#include "int.h"
#include "value.h"

enum class Opcode
{
    Add,
    Constant,
    ConstantExt,
    Divide,
    Equal,
    False,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    Modulo,
    Multiply,
    Negate,
    Not,
    NotEqual,
    Null,
    Return,
    Subtract,
    True,
};

class Chunk
{
public:
    Chunk();

    void write(u8 byte, std::size_t line);
    void write(Opcode opcode, std::size_t line);

    std::size_t line(std::size_t index);

    Values constants;
    std::vector<u8> code;

private:
    struct Line
    {
        std::size_t index;
        std::size_t number;
    };
    std::vector<Line> lines;
};
