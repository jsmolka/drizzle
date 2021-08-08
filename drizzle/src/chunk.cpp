#include "chunk.h"

#include <limits>

#include "error.h"

void Chunk::write(Opcode opcode)
{
    write(static_cast<u8>(opcode));
}

void Chunk::writeConstant(const Value& value)
{
    auto index = constants.size();
    if (index <= std::numeric_limits<u8>::max())
    {
        write(Opcode::ConstantByte);
        write<u8>(index);
    }
    else if (index <= std::numeric_limits<u16>::max())
    {
        write(Opcode::ConstantHalf);
        write<u16>(index);
    }
    else if (index <= std::numeric_limits<u32>::max())
    {
        write(Opcode::ConstantWord);
        write<u32>(index);
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
        index = opcode(index);
    }
}

std::size_t Chunk::opcode(std::size_t index)
{
    fmt::print("{:08}  ", index);

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
        fmt::print("Unknown opcode {}\n", opcode);
        return index;
    }
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

    fmt::print("{} [{}] -> {}\n", name, constant, constants[constant]);
    return index;
}

std::size_t Chunk::opcodePrimitive(std::string_view name, std::size_t index)
{
    fmt::print("{}\n", name);
    return index;
}
