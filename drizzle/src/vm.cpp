#include "vm.h"

#include <functional>
#include <shell/format.h>
#include <shell/punning.h>

#include "compiler.h"
#include "error.h"

void Vm::interpret(const Tokens& tokens)
{
    Chunk c;
    Compiler compiler;
    compiler.compile(tokens, c);

    chunk = &c;
    ip = c.code.data();

    run();

    shell::print(stack[0]);
}

template<typename Integral>
Integral Vm::read()
{
    static_assert(std::is_integral_v<Integral>);

    auto value = shell::read<Integral>(ip, 0);
    ip += sizeof(Integral);
    return value;
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
    // - add opcodes for frequent numbers -1, 0, 1, 2

    while (true)
    {
        auto opcode = static_cast<Opcode>(read<u8>());
        switch (opcode)
        {
        case Opcode::Constant: stack.push(chunk->constants[read<u8>()]); break;
        case Opcode::ConstantExt: stack.push(chunk->constants[read<u16>()]); break;
        case Opcode::Add: add(); break;
        case Opcode::Subtract: subtract(); break;
        case Opcode::Multiply: multiply(); break;
        case Opcode::Divide: divide(); break;
        case Opcode::Negate: negate(); break;
        case Opcode::Not: not(); break;
        case Opcode::True: stack.push(true); break;
        case Opcode::False: stack.push(false); break;
        case Opcode::Null: stack.push({}); break;
        case Opcode::Equal: equal(); break;
        case Opcode::NotEqual: notEqual(); break;
        case Opcode::Less: less(); break;
        case Opcode::LessEqual: lessEqual(); break;
        case Opcode::Greater: greater(); break;
        case Opcode::GreaterEqual: greaterEqual(); break;
        case Opcode::Return: return;
        }
    }
}

void Vm::negate()
{
    auto& value = stack[0];
    switch (value.type)
    {
    case Value::Type::Boolean:
        value.integer = -static_cast<s64>(value.boolean);
        value.type = Value::Type::Integer;
        break;

    case Value::Type::Integer:
        value.integer = -value.integer;
        break;

    case Value::Type::Rational:
        value.rational = -value.rational;
        break;

    default:
        throw TypeError("UNARY, Todo: something smart");
    }
}

void Vm::not()
{
    auto& value = stack[0];
    value.boolean = !static_cast<bool>(value);
    value.type = Value::Type::Boolean;
}

void Vm::add()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    lhs.rational += rhs.rational;
}

void Vm::subtract()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    lhs.rational -= rhs.rational;
}

void Vm::multiply()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    lhs.rational *= rhs.rational;
}

void Vm::divide()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    lhs.rational /= rhs.rational;
}

void Vm::equal()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];
    lhs.boolean = lhs == rhs;
    lhs.type = Value::Type::Boolean;
}

void Vm::notEqual()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];
    lhs.boolean = lhs != rhs;
    lhs.type = Value::Type::Boolean;
}

void Vm::less()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];
    lhs.boolean = lhs.rational < rhs.rational;
    lhs.type = Value::Type::Boolean;
}

void Vm::lessEqual()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];
    lhs.boolean = lhs.rational <= rhs.rational;
    lhs.type = Value::Type::Boolean;
}

void Vm::greater()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];
    lhs.boolean = lhs.rational > rhs.rational;
    lhs.type = Value::Type::Boolean;
}

void Vm::greaterEqual()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];
    lhs.boolean = lhs.rational >= rhs.rational;
    lhs.type = Value::Type::Boolean;
}
