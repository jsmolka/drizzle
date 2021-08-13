#pragma once

#include <string_view>

#include "value.h"

enum class Opcode : u8
{
    Constant,
    ConstantExt,
    Add,
    Subtract,
    Multiply,
    Divide,
    Negate,
    Return
};

class Chunk
{
public:
    void write(Opcode opcode, std::size_t line);
    void writeConstant(const Value& value, std::size_t line);

    void disassemble() const;
    std::size_t disassembleAt(std::size_t index) const;
    std::size_t lineAt(std::size_t index) const;

    Values constants;
    std::vector<u8> code;

private:
    struct Line
    {
        std::size_t begin;
        std::size_t number;
    };

    template<typename Integral>
    Integral read(std::size_t index) const;

    template<typename Integral>
    void write(Integral value, std::size_t line);

    std::size_t opcodeConstant(Opcode opcode, std::size_t index) const;
    std::size_t opcodePrimitive(std::string_view name, std::size_t index) const;

    std::vector<Line> lines;
};
