#pragma once

#include <string_view>

#include "value.h"

enum class Opcode : u8
{
    ConstantByte,  // For byte indices
    ConstantHalf,  // For halfword indices
    ConstantWord,  // For word indices
    Return
};

class Chunk
{
public:
    template<typename Integral>
    Integral read(std::size_t index) const
    {
        static_assert(std::is_integral_v<Integral>);

        return *reinterpret_cast<const Integral*>(&code.data()[index]);
    }

    template<typename Integral>
    void write(Integral value)
    {
        static_assert(std::is_integral_v<Integral>);

        code.push_back(value);
        if constexpr (sizeof(Integral) >= 2) code.push_back(value >> 8);
        if constexpr (sizeof(Integral) >= 3) code.push_back(value >> 16);
        if constexpr (sizeof(Integral) >= 4) code.push_back(value >> 24);
        if constexpr (sizeof(Integral) >= 5) code.push_back(value >> 32);
        if constexpr (sizeof(Integral) >= 6) code.push_back(value >> 40);
        if constexpr (sizeof(Integral) >= 7) code.push_back(value >> 48);
        if constexpr (sizeof(Integral) >= 8) code.push_back(value >> 56);
    }

    void write(Opcode opcode);
    void writeConstant(const Value& value);

    void disassemble();

private:
    std::size_t opcode(std::size_t index);
    std::size_t opcodeConstant(Opcode opcode, std::size_t index);
    std::size_t opcodePrimitive(std::string_view name, std::size_t index);

    Values constants;
    std::vector<u8> code;
};
