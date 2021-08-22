#include "chunk.h"

void Chunk::write(u8 byte)
{
    code.push_back(byte);
}

void Chunk::write(Opcode opcode)
{
    write(static_cast<u8>(opcode));
}
