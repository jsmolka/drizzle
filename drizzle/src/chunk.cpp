#include "chunk.h"

#include <limits>
#include <shell/format.h>
#include <shell/punning.h>
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
        write(Opcode::Constant, line);
        write<u8>(index, line);
    }
    else if (index <= std::numeric_limits<u16>::max())
    {
        write(Opcode::ConstantExt, line);
        write<u16>(index, line);
    }
    else
    {
        throw Error("Constant limit exceeded");
    }
    constants.push_back(value);
}

void Chunk::disassemble() const
{
    for (std::size_t index = 0; index < code.size(); )
    {
        index = disassembleAt(index);
    }
}

std::size_t Chunk::disassembleAt(std::size_t index) const
{
    shell::print("{:04} {:04} ", index, lineAt(index));

    auto opcode = static_cast<Opcode>(read<u8>(index++));
    switch (opcode)
    {
    case Opcode::Constant:
    case Opcode::ConstantExt:
        return opcodeConstant(opcode, index);

    case Opcode::Add:
        return opcodePrimitive("Opcode::Add", index);

    case Opcode::Subtract:
        return opcodePrimitive("Opcode::Subtract", index);

    case Opcode::Multiply:
        return opcodePrimitive("Opcode::Multiply", index);

    case Opcode::Divide:
        return opcodePrimitive("Opcode::Divide", index);

    case Opcode::Negate:
        return opcodePrimitive("Opcode::Negate", index);

    case Opcode::Return:
        return opcodePrimitive("Opcode::Return", index);

    default:
        shell::print("Unknown opcode {}\n", opcode);
        return index;
    }
}

std::size_t Chunk::lineAt(std::size_t index) const
{
    for (const auto& line : shell::reversed(lines))
    {
        if (index >= line.begin)
            return line.number;
    }
    return 0;
}

template<typename Integral>
Integral Chunk::read(std::size_t index) const
{
    static_assert(std::is_integral_v<Integral>);

    return shell::read<Integral>(code.data(), index);
}

template<typename Integral>
void Chunk::write(Integral value, std::size_t line)
{
    static_assert(std::is_integral_v<Integral>);

    if (lines.empty() || line > lines.back().number)
        lines.push_back({ code.size(), line });

    code.push_back(value);
    if constexpr (sizeof(Integral) >= 2) code.push_back(value >> 8);
    if constexpr (sizeof(Integral) >= 3) code.push_back(value >> 16);
    if constexpr (sizeof(Integral) >= 4) code.push_back(value >> 24);
    if constexpr (sizeof(Integral) >= 5) code.push_back(value >> 32);
    if constexpr (sizeof(Integral) >= 6) code.push_back(value >> 40);
    if constexpr (sizeof(Integral) >= 7) code.push_back(value >> 48);
    if constexpr (sizeof(Integral) >= 8) code.push_back(value >> 56);
}

std::size_t Chunk::opcodeConstant(Opcode opcode, std::size_t index) const
{
    std::string_view name;
    std::size_t constant = 0;

    switch (opcode)
    {
    case Opcode::Constant:
        name = "Opcode::Constant";
        constant = read<u8>(index);
        index += 1;
        break;
        
    case Opcode::ConstantExt:
        name = "Opcode::ConstantExt";
        constant = read<u16>(index);
        index += 2;
        break;
    }

    shell::print("{} [{}] -> {}\n", name, constant, constants[constant]);
    return index;
}

std::size_t Chunk::opcodePrimitive(std::string_view name, std::size_t index) const
{
    shell::print("{}\n", name);
    return index;
}
