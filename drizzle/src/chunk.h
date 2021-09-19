#pragma once

#include "int.h"
#include "opcode.h"
#include "value.h"

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
