#include "chunk.h"

#include <shell/ranges.h>

void Chunk::write(u8 byte, std::size_t line)
{
    if (lines.empty() || line > lines.back().number)
        lines.push_back({ code.size(), line });

    code.push_back(byte);
}

std::size_t Chunk::label() const
{
    return code.size();
}

std::size_t Chunk::line(std::size_t index) const
{
    for (const auto& line : shell::reversed(lines))
    {
        if (line.index < index)
            return line.number;
    }
    return -1;
}
