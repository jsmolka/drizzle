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

template<typename Callback>
auto Vm::promote(const Value& op1, const Value& op2, Callback callback)
{
    SHELL_ASSERT(op1.isPrimitive() && op2.isPrimitive());

    auto promote = [callback](auto a, auto b)
    {
        using T = decltype(a);
        using U = decltype(b);
        using P = promoted_t<T, U>;

        return callback(static_cast<P>(a), static_cast<P>(b));
    };

    #define HASH(type1, type2) (int(type1) << 16) | int(type2)

    switch (HASH(op1.type, op2.type))
    {
    case HASH(Value::Type::Int,   Value::Type::Int  ): return promote(op1.i, op2.i);
    case HASH(Value::Type::Int,   Value::Type::Float): return promote(op1.i, op2.f);
    case HASH(Value::Type::Int,   Value::Type::Bool ): return promote(op1.i, op2.b);
    case HASH(Value::Type::Float, Value::Type::Int  ): return promote(op1.f, op2.i);
    case HASH(Value::Type::Float, Value::Type::Float): return promote(op1.f, op2.f);
    case HASH(Value::Type::Float, Value::Type::Bool ): return promote(op1.f, op2.b);
    case HASH(Value::Type::Bool,  Value::Type::Int  ): return promote(op1.b, op2.i);
    case HASH(Value::Type::Bool,  Value::Type::Float): return promote(op1.b, op2.f);
    case HASH(Value::Type::Bool,  Value::Type::Bool ): return promote(op1.b, op2.b);

    default:
        SHELL_UNREACHABLE;
        return callback(dzint{}, dzint{});
    }

    #undef HASH
}

void Vm::requirePrimitive(const Value& lhs, const Value& rhs, const char* error)
{
    if (!(lhs.isPrimitive() && rhs.isPrimitive()))
        throw TypeError(error);
}

std::tuple<Value&, Value> Vm::binaryOperands()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    return std::forward_as_tuple(lhs, rhs);
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
        case Opcode::Modulo: modulo(); break;
        case Opcode::Return: return;
        }
    }
}

void Vm::negate()
{
    auto& value = stack[0];
    switch (value.type)
    {
    case Value::Type::Bool:
        value.i = -static_cast<s64>(value.b);
        value.type = Value::Type::Int;
        break;

    case Value::Type::Int:
        value.i = -value.i;
        break;

    case Value::Type::Float:
        value.f = -value.f;
        break;

    default:
        throw TypeError("UNARY, Todo: something smart");
    }
}

void Vm::not()
{
    auto& value = stack[0];
    value.set(!static_cast<bool>(value));
}

void Vm::add()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'+' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b) { lhs.set(a + b); });
}

void Vm::subtract()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'-' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b) { lhs.set(a - b); });
}

void Vm::multiply()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'*' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b) { lhs.set(a * b); });
}

void Vm::divide()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'/' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b) {
        if (b == decltype(b)(0))
            throw DivisionByZeroError("fak");
        lhs.set(a / b);
    });
}

void Vm::equal()
{
    auto [lhs, rhs] = binaryOperands();
    lhs.set(lhs == rhs);
}

void Vm::notEqual()
{
    auto [lhs, rhs] = binaryOperands();
    lhs.set(lhs != rhs);
}

void Vm::modulo()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'%' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b)
    {
        using T = decltype(a);
        if constexpr (std::is_same_v<T, dzint>)
            lhs.set(a % b);
        if constexpr (std::is_same_v<T, dzfloat>)
            lhs.set(std::fmod(a, b));
    });
}

void Vm::less()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'<' requires primitive operands");
    lhs.set(promote(lhs, rhs, [](auto a, auto b) { return a < b; }));
}

void Vm::lessEqual()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'<=' requires primitive operands");
    lhs.set(promote(lhs, rhs, [](auto a, auto b) { return a <= b; }));
}

void Vm::greater()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'>' requires primitive operands");
    lhs.set(promote(lhs, rhs, [](auto a, auto b) { return a > b; }));
}

void Vm::greaterEqual()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'>=' requires primitive operands");
    lhs.set(promote(lhs, rhs, [](auto a, auto b) { return a >= b; }));
}
