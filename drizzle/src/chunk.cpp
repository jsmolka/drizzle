#include "chunk.h"

void Chunk::write(u8 byte, std::size_t line)
{
    code.push_back(byte);
    lines.push_back(line);
}

void Chunk::write(Opcode opcode, std::size_t line)
{
    write(static_cast<u8>(opcode), line);
}
