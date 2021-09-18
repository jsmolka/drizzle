#include "vm.h"

#include <functional>
#include <shell/format.h>
#include <shell/punning.h>

#include "compiler.h"
#include "errors.h"

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

template<typename Operation>
void Vm::binary(Value& lhs, const Value& rhs, Operation op)
{
    auto promote = [op](auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);
        using P = promoted_t<A, B>;

        return op(static_cast<P>(a), static_cast<P>(b));
    };

    #define HASH(a, b) ((int(a) << 16) | int(b))

    switch (HASH(lhs.type, rhs.type))
    {
    case HASH(Value::Type::Int,   Value::Type::Int  ): lhs.set(promote(lhs.i, rhs.i)); break;
    case HASH(Value::Type::Int,   Value::Type::Float): lhs.set(promote(lhs.i, rhs.f)); break;
    case HASH(Value::Type::Int,   Value::Type::Bool ): lhs.set(promote(lhs.i, rhs.b)); break;
    case HASH(Value::Type::Float, Value::Type::Int  ): lhs.set(promote(lhs.f, rhs.i)); break;
    case HASH(Value::Type::Float, Value::Type::Float): lhs.set(promote(lhs.f, rhs.f)); break;
    case HASH(Value::Type::Float, Value::Type::Bool ): lhs.set(promote(lhs.f, rhs.b)); break;
    case HASH(Value::Type::Bool,  Value::Type::Int  ): lhs.set(promote(lhs.b, rhs.i)); break;
    case HASH(Value::Type::Bool,  Value::Type::Float): lhs.set(promote(lhs.b, rhs.f)); break;
    case HASH(Value::Type::Bool,  Value::Type::Bool ): lhs.set(promote(lhs.b, rhs.b)); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    #undef HASH
}

void Vm::requirePrimitive(const Value& lhs, const Value& rhs, const char* error)
{
    if (!(lhs.isPrimitive() && rhs.isPrimitive()))
        throw TypeError(error, 0);
}

std::tuple<Value&, Value> Vm::operands()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    return std::forward_as_tuple(lhs, rhs);
}

void Vm::run()
{
    while (true)
    {
        switch (static_cast<Opcode>(read<u8>()))
        {
        case Opcode::Add: add(); break;
        case Opcode::Constant: constant(); break;
        case Opcode::ConstantExt: constantExt(); break;
        case Opcode::Divide: divide(); break;
        case Opcode::Equal: equal(); break;
        case Opcode::False: valueFalse(); break;
        case Opcode::Greater: greater(); break;
        case Opcode::GreaterEqual: greaterEqual(); break;
        case Opcode::Less: less(); break;
        case Opcode::LessEqual: lessEqual(); break;
        case Opcode::Modulo: modulo(); break;
        case Opcode::Multiply: multiply(); break;
        case Opcode::Negate: negate(); break;
        case Opcode::Not: not(); break;
        case Opcode::NotEqual: notEqual(); break;
        case Opcode::Null: valueNull(); break;
        case Opcode::Return: return;
        case Opcode::Subtract: subtract(); break;
        case Opcode::True: valueTrue(); break;
        }
    }
}

void Vm::add()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'+' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b) { return a + b; });
}

void Vm::constant()
{
    stack.push(chunk->constants[read<u8>()]);
}

void Vm::constantExt()
{
    stack.push(chunk->constants[read<u16>()]);
}

void Vm::divide()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'/' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b)
    {
        using T = decltype(b);
        if (b == static_cast<T>(0))
            throw ZeroDivsionError("Todo", 0);
        return a / b;
    });
}

void Vm::equal()
{
    auto [lhs, rhs] = operands();
    lhs.set(lhs == rhs);
}

void Vm::greater()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'>' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b) { return a > b; });
}

void Vm::greaterEqual()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'>=' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b) { return a >= b; });
}

void Vm::less()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'<' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b) { return a < b; });
}

void Vm::lessEqual()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'<=' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b) { return a <= b; });
}

void Vm::modulo()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'%' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b)
    {
        using T = decltype(b);
        if (b == static_cast<T>(0))
            throw ZeroDivsionError("Todo", 0);
        if constexpr (std::is_same_v<T, dzint>)
            return a % b;
        if constexpr (std::is_same_v<T, dzfloat>)
            return std::fmod(a, b);
    });
}

void Vm::multiply()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'*' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b) { return a * b; });
}

void Vm::negate()
{
    auto& value = stack[0];
    switch (value.type)
    {
    case Value::Type::Bool:  value.set(-static_cast<dzint>(value.b)); break;
    case Value::Type::Int:   value.set(-value.i); break;
    case Value::Type::Float: value.set(-value.f); break;

    default:
        throw TypeError("'-' requires primitive operand", 0);
    }
}

void Vm::not()
{
    auto& value = stack[0];
    value.set(!static_cast<bool>(value));
}

void Vm::notEqual()
{
    auto [lhs, rhs] = operands();
    lhs.set(lhs != rhs);
}

void Vm::subtract()
{
    auto [lhs, rhs] = operands();
    requirePrimitive(lhs, rhs, "'-' requires primitive operands");
    binary(lhs, rhs, [](auto a, auto b) { return a - b; });
}

void Vm::valueFalse()
{
    stack.push(false);
}

void Vm::valueNull()
{
    stack.push({});
}

void Vm::valueTrue()
{
    stack.push(true);
}
