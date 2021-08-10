#include <shell/format.h>
#include <shell/main.h>

#include "chunk.h"
#include "stack.h"
#include "vm.h"

int main(int argc, char* argv[])
{
    try
    {
        Chunk chunk;
        chunk.writeConstant(1, 1);
        chunk.writeConstant(2, 1);
        chunk.write(Opcode::Add, 1);
        chunk.writeConstant(1.5, 1);
        chunk.write(Opcode::Divide, 1);
        chunk.write(Opcode::Return, 1);
        vm.interpret(chunk);

        return 0;
    }
    catch (const std::exception& ex)
    {
        shell::print(ex.what());
        return 1;
    }
}
