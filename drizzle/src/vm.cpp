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
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'+' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b) { lhs.set(a + b); });
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
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'/' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b)
    {
        using T = decltype(b);
        if (b == static_cast<T>(0))
            throw ZeroDivsionError("Todo");
        lhs.set(a / b);
    });
}

void Vm::equal()
{
    auto [lhs, rhs] = binaryOperands();
    lhs.set(lhs == rhs);
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

void Vm::modulo()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'%' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b)
    {
        using T = decltype(b);
        if (b == static_cast<T>(0))
            throw ZeroDivsionError("Todo");
        if constexpr (std::is_same_v<T, dzint>)
            lhs.set(a % b);
        if constexpr (std::is_same_v<T, dzfloat>)
            lhs.set(std::fmod(a, b));
    });
}

void Vm::multiply()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'*' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b) { lhs.set(a * b); });
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
        throw TypeError("'-' requires primitive operand");
    }
}

void Vm::not()
{
    auto& value = stack[0];
    value.set(!static_cast<bool>(value));
}

void Vm::notEqual()
{
    auto [lhs, rhs] = binaryOperands();
    lhs.set(lhs != rhs);
}

void Vm::subtract()
{
    auto [lhs, rhs] = binaryOperands();
    requirePrimitive(lhs, rhs, "'-' requires primitive operands");
    promote(lhs, rhs, [&lhs = lhs](auto a, auto b) { lhs.set(a - b); });
}

void Vm::valueTrue()
{
    stack.push(true);
}

void Vm::valueFalse()
{
    stack.push(false);
}

void Vm::valueNull()
{
    stack.push({});
}
