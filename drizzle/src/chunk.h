#pragma once

#include "dzvalue.h"
#include "opcode.h"

class Chunk
{
public:
    Chunk();

    void write(u8 byte, std::size_t line);
    void write(Opcode opcode, std::size_t line);

    std::size_t label() const;
    std::size_t line(std::size_t index) const;

    DzValues constants;
    std::vector<u8> code;

private:
    struct Line
    {
        std::size_t index;
        std::size_t number;
    };
    std::vector<Line> lines;
};
