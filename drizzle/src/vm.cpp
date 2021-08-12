#include "vm.h"

#include <functional>
#include <shell/format.h>
#include <shell/punning.h>

#include "compiler.h"

#define TRACE_EXECUTION

void Vm::interpret(std::string_view source)
{
    Compiler compiler;
    compiler.compile(source);
}

template<typename Integral>
Integral Vm::read()
{
    static_assert(std::is_integral_v<Integral>);

    auto value = shell::read<Integral>(ip, 0);
    ip += sizeof(Integral);
    return value;
}

template<typename Operator>
void Vm::binaryOperation()
{
    const auto rhs = stack.pop();
    const auto lhs = stack.pop();
    stack.push(Operator()(lhs, rhs));
}

void Vm::run()
{
    // Possible performance improvements:
    // - Direct threaded code
    // - Jump tables
    // - Computed goto
    // - Direct stack manipulation (Opcode::Negative without push/pop)
    // - Register based bytecode
    // - Keep IP in CPU register, no member variable

    while (true)
    {
        #ifdef TRACE_EXECUTION
        chunk->disassembleAt(ip - chunk->code.data());
        for (const auto& value : stack)
            shell::print("          [{}]\n", value);
        #endif

        auto opcode = static_cast<Opcode>(read<u8>());
        switch (opcode)
        {
        case Opcode::Constant:
            stack.push(chunk->constants[read<u8>()]);
            break;

        case Opcode::ConstantExt:
            stack.push(chunk->constants[read<u16>()]);
            break;

        case Opcode::Add:
            binaryOperation<std::plus<Value>>();
            break;

        case Opcode::Subtract:
            binaryOperation<std::minus<Value>>();
            break;

        case Opcode::Multiply:
            binaryOperation<std::multiplies<Value>>();
            break;

        case Opcode::Divide:
            binaryOperation<std::divides<Value>>();
            break;

        case Opcode::Negate:
            stack.push(-stack.pop());
            break;

        case Opcode::Return:
            return;
        }
    }
}
