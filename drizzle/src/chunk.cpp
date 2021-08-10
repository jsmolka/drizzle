#include "chunk.h"

#include <limits>
#include <shell/format.h>
#include <shell/ranges.h>

#include "error.h"

void Chunk::write(Opcode opcode, std::size_t line)
{
    write(static_cast<u8>(opcode), line);
}

void Chunk::writeConstant(const Value& value, std::size_t line)
{
    auto index = constants.size();
    if (index <= std::numeric_limits<u8>::max())
    {
        write(Opcode::ConstantByte, line);
        write<u8>(index, line);
    }
    else if (index <= std::numeric_limits<u16>::max())
    {
        write(Opcode::ConstantHalf, line);
        write<u16>(index, line);
    }
    else if (index <= std::numeric_limits<u32>::max())
    {
        write(Opcode::ConstantWord, line);
        write<u32>(index, line);
    }
    else
    {
        throw Error("Constant limit exceeded");
    }
    constants.push_back(value);
}

void Chunk::disassemble()
{
    for (std::size_t index = 0; index < code.size(); )
    {
        index = disassembleAt(index);
    }
}

std::size_t Chunk::disassembleAt(std::size_t index)
{
    shell::print("{:04}  {:04}  ", index, lineAt(index));

    auto opcode = static_cast<Opcode>(read<u8>(index++));
    switch (opcode)
    {
    case Opcode::ConstantByte:
    case Opcode::ConstantHalf:
    case Opcode::ConstantWord:
        return opcodeConstant(opcode, index);

    case Opcode::Return:
        return opcodePrimitive("Opcode::Return", index);

    default:
        shell::print("Unknown opcode {}\n", opcode);
        return index;
    }
}

std::size_t Chunk::lineAt(std::size_t index)
{
    for (const auto& line : shell::reversed(lines))
    {
        if (index >= line.begin)
            return line.number;
    }
    return 0;
}

std::size_t Chunk::opcodeConstant(Opcode opcode, std::size_t index)
{
    std::string_view name;
    std::size_t constant = 0;

    switch (opcode)
    {
    case Opcode::ConstantByte:
        name = "Opcode::ConstantByte";
        constant = read<u8>(index);
        index += 1;
        break;
        
    case Opcode::ConstantHalf:
        name = "Opcode::ConstantHalf";
        constant = read<u16>(index);
        index += 2;
        break;

    case Opcode::ConstantWord:
        name = "Opcode::ConstantWord";
        constant = read<u32>(index);
        index += 4;
        break;
    }

    shell::print("{} [{}] -> {}\n", name, constant, constants[constant]);
    return index;
}

std::size_t Chunk::opcodePrimitive(std::string_view name, std::size_t index)
{
    shell::print("{}\n", name);
    return index;
}
