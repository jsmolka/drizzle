#include <shell/format.h>
#include <shell/main.h>

#include "chunk.h"

int main(int argc, char* argv[])
{
    try
    {
        Chunk chunk;
        chunk.write(Opcode::Return, 1);
        chunk.writeConstant(10, 1);
        chunk.writeConstant(11, 1);
        chunk.writeConstant(12, 2);
        chunk.writeConstant(10, 3);
        chunk.disassemble();

        return 0;
    }
    catch (const std::exception& ex)
    {
        shell::print(ex.what());
        return 1;
    }
}
