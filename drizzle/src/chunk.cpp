#include "chunk.h"

#include <shell/ranges.h>

Chunk::Chunk()
{
    lines.push_back({ 0, 0 });
}

void Chunk::write(u8 byte, std::size_t line)
{
    code.push_back(byte);

    if (line > lines.back().number)
        lines.push_back({ code.size(), line });
}

void Chunk::write(Opcode opcode, std::size_t line)
{
    write(static_cast<u8>(opcode), line);
}

std::size_t Chunk::label() const
{
    return code.size();
}

std::size_t Chunk::line(std::size_t index)
{
    for (const auto& line : shell::reversed(lines))
    {
        if (line.index <= index)
            return line.number;
    }
    return -1;
}
