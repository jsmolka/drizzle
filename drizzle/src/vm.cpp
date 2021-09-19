#include "vm.h"

#include <shell/punning.h>

#include "compiler.h"
#include "errors.h"

void Vm::interpret(const Tokens& tokens)
{
    Compiler compiler;
    compiler.compile(tokens, chunk);

    ip = chunk.code.data();

    while (true)
    {
        switch (static_cast<Opcode>(read<u8>()))
        {
        case Opcode::Add: add(); break;
        case Opcode::Constant: constant(); break;
        case Opcode::ConstantExt: constantExt(); break;
        case Opcode::Divide: divide(); break;
        case Opcode::DivideInt: divideInt(); break;
        case Opcode::Equal: equal(); break;
        case Opcode::Exit: goto end;
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
        case Opcode::Subtract: subtract(); break;
        case Opcode::True: valueTrue(); break;
        }
    }

end:
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

template<typename Error, typename ...Args>
void Vm::raise(const std::string& message, Args&& ...args)
{
    static_assert(std::is_base_of_v<RuntimeError, Error>);

    std::size_t index = ip - chunk.code.data();
    std::size_t line = chunk.line(index);

    throw Error(shell::format(message, std::forward<Args>(args)...), line);
}

template<typename Operation>
void Vm::primitiveBinary(Value& lhs, const Value& rhs, Operation op)
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

std::tuple<Value&, Value> Vm::operands()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    return std::forward_as_tuple(lhs, rhs);
}

std::tuple<Value&, Value> Vm::primitiveOperands(std::string_view operation)
{
    auto [lhs, rhs] = operands();
    
    if (!(lhs.isPrimitive() && rhs.isPrimitive()))
        raise<TypeError>("unsupported operand types for '{}': '{}' and '{}'", operation, lhs.typeName(), rhs.typeName());

    return std::forward_as_tuple(lhs, rhs);
}

void Vm::add()
{
    auto [lhs, rhs] = primitiveOperands("+");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a + b; });
}

void Vm::constant()
{
    stack.push(chunk.constants[read<u8>()]);
}

void Vm::constantExt()
{
    stack.push(chunk.constants[read<u16>()]);
}

void Vm::divide()
{
    auto [lhs, rhs] = primitiveOperands("/");
    primitiveBinary(lhs, rhs, [this](auto a, auto b)
    {
        using T = decltype(b);
        if (b == static_cast<T>(0))
            raise<ZeroDivsionError>("division by zero");
        return a / b;
    });
}

void Vm::divideInt()
{
    auto [lhs, rhs] = primitiveOperands("//");
    primitiveBinary(lhs, rhs, [this](auto a, auto b)
    {
        using T = decltype(b);
        if (b == static_cast<T>(0))
            raise<ZeroDivsionError>("integer division by zero");
        if constexpr (std::is_same_v<T, dzint>)
            return a / b;
        if constexpr (std::is_same_v<T, dzfloat>)
            return std::floor(a / b);
    });
}

void Vm::equal()
{
    auto [lhs, rhs] = operands();
    lhs.set(lhs == rhs);
}

void Vm::greater()
{
    auto [lhs, rhs] = primitiveOperands(">");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a > b; });
}

void Vm::greaterEqual()
{
    auto [lhs, rhs] = primitiveOperands(">=");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a >= b; });
}

void Vm::less()
{
    auto [lhs, rhs] = primitiveOperands("<");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a < b; });
}

void Vm::lessEqual()
{
    auto [lhs, rhs] = primitiveOperands("<=");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a <= b; });
}

void Vm::modulo()
{
    auto [lhs, rhs] = primitiveOperands("%");
    primitiveBinary(lhs, rhs, [this](auto a, auto b)
    {
        using T = decltype(b);
        if (b == static_cast<T>(0))
            raise<ZeroDivsionError>("modulo by zero");
        if constexpr (std::is_same_v<T, dzint>)
            return a % b;
        if constexpr (std::is_same_v<T, dzfloat>)
            return std::fmod(a, b);
    });
}

void Vm::multiply()
{
    auto [lhs, rhs] = primitiveOperands("*");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a * b; });
}

void Vm::negate()
{
    auto& value = stack[0];
    switch (value.type)
    {
    case Value::Type::Int:   value.set(-value.i); break;
    case Value::Type::Float: value.set(-value.f); break;
    case Value::Type::Bool:  value.set(-static_cast<dzint>(value.b)); break;

    default:
        raise<TypeError>("unsupported operand type for '-': {}", value.typeName());
        break;
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
    auto [lhs, rhs] = primitiveOperands("-");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a - b; });
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
