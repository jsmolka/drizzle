#include "vm.h"

#include <shell/punning.h>

#include "compiler.h"
#include "dzstring.h"
#include "errors.h"
#include "format.h"

void Vm::interpret(const Tokens& tokens)
{
    Compiler compiler(interning);
    compiler.compile(tokens, chunk);

    ip = chunk.code.data();

    while (true)
    {
        switch (static_cast<Opcode>(read<u8>()))
        {
        case Opcode::Add: add(); break;
        case Opcode::Assert: assertion(); break;
        case Opcode::BitAnd: bitAnd(); break;
        case Opcode::BitAsr: bitAsr(); break;
        case Opcode::BitComplement: bitComplement(); break;
        case Opcode::BitLsr: bitLsr(); break;
        case Opcode::BitLsl: bitLsl(); break;
        case Opcode::BitOr: bitOr(); break;
        case Opcode::BitXor: bitXor(); break;
        case Opcode::Constant: constant(); break;
        case Opcode::ConstantExt: constantExt(); break;
        case Opcode::Discard: discard(); break;
        case Opcode::Divide: divide(); break;
        case Opcode::DivideInt: divideInt(); break;
        case Opcode::Equal: equal(); break;
        case Opcode::Exit: return;
        case Opcode::False: valueFalse(); break;
        case Opcode::Greater: greater(); break;
        case Opcode::GreaterEqual: greaterEqual(); break;
        case Opcode::Less: less(); break;
        case Opcode::LessEqual: lessEqual(); break;
        case Opcode::Modulo: modulo(); break;
        case Opcode::Multiply: multiply(); break;
        case Opcode::Negate: negate(); break;
        case Opcode::Not: not_(); break;
        case Opcode::NotEqual: notEqual(); break;
        case Opcode::Null: valueNull(); break;
        case Opcode::Power: power(); break;
        case Opcode::Print: print(); break;
        case Opcode::Subtract: subtract(); break;
        case Opcode::True: valueTrue(); break;

        default:
            SHELL_UNREACHABLE;
            break;
        }
    }
}

template<typename Integral>
Integral Vm::read()
{
    static_assert(std::is_integral_v<Integral>);

    const auto value = shell::read<Integral>(ip, 0);
    ip += sizeof(Integral);
    return value;
}

template<typename Error, typename ...Args>
void Vm::raise(std::string_view message, Args&& ...args)
{
    static_assert(std::is_base_of_v<RuntimeError, Error>);

    std::size_t index = ip - chunk.code.data();
    std::size_t line = chunk.line(index);

    throw Error(line, message, std::forward<Args>(args)...);
}

template<typename Operation>
void Vm::bitwiseBinary(DzValue& lhs, const DzValue& rhs, Operation operation)
{
    auto promote = [operation](auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);
        using P = promoted_t<A, B>;

        return operation(static_cast<P>(a), static_cast<P>(b));
    };

    #define HASH(a, b) ((int(a) << 1) | int(b))

    switch (HASH(lhs.type, rhs.type))
    {
    case HASH(DzValue::Type::Int,   DzValue::Type::Int ): lhs.set(promote(lhs.i, rhs.i)); break;
    case HASH(DzValue::Type::Int,   DzValue::Type::Bool): lhs.set(promote(lhs.i, rhs.b)); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Int ): lhs.set(promote(lhs.b, rhs.i)); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Bool): lhs.set(promote(lhs.b, rhs.b)); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    #undef HASH
}

template<typename Operation>
void Vm::primitiveBinary(DzValue& lhs, const DzValue& rhs, Operation operation)
{
    auto promote = [operation](auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);
        using P = promoted_t<A, B>;

        return operation(static_cast<P>(a), static_cast<P>(b));
    };

    #define HASH(a, b) ((int(a) << 2) | int(b))

    switch (HASH(lhs.type, rhs.type))
    {
    case HASH(DzValue::Type::Int,   DzValue::Type::Int  ): lhs.set(promote(lhs.i, rhs.i)); break;
    case HASH(DzValue::Type::Int,   DzValue::Type::Float): lhs.set(promote(lhs.i, rhs.f)); break;
    case HASH(DzValue::Type::Int,   DzValue::Type::Bool ): lhs.set(promote(lhs.i, rhs.b)); break;
    case HASH(DzValue::Type::Float, DzValue::Type::Int  ): lhs.set(promote(lhs.f, rhs.i)); break;
    case HASH(DzValue::Type::Float, DzValue::Type::Float): lhs.set(promote(lhs.f, rhs.f)); break;
    case HASH(DzValue::Type::Float, DzValue::Type::Bool ): lhs.set(promote(lhs.f, rhs.b)); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Int  ): lhs.set(promote(lhs.b, rhs.i)); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Float): lhs.set(promote(lhs.b, rhs.f)); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Bool ): lhs.set(promote(lhs.b, rhs.b)); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    #undef HASH
}

void Vm::raiseTypeError(std::string_view operation, const DzValue& value)
{
    raise<TypeError>("bad operand type for '{}': '{}'", operation, value.typeName());
}

void Vm::raiseTypeError(std::string_view operation, const DzValue& lhs, const DzValue& rhs)
{
    raise<TypeError>("bad operand types for '{}': '{}' and '{}'", operation, lhs.typeName(), rhs.typeName());
}

std::tuple<DzValue&, DzValue> Vm::operands()
{
    auto  rhs = stack.pop();
    auto& lhs = stack[0];

    return std::forward_as_tuple(lhs, rhs);
}

std::tuple<DzValue&, DzValue> Vm::bitwiseOperands(std::string_view operation)
{
    auto [lhs, rhs] = operands();

    if (!(lhs.isBitwise() && rhs.isBitwise()))
        raiseTypeError(operation, lhs, rhs);

    return std::forward_as_tuple(lhs, rhs);
}

std::tuple<DzValue&, DzValue> Vm::primitiveOperands(std::string_view operation)
{
    auto [lhs, rhs] = operands();
    
    if (!(lhs.isPrimitive() && rhs.isPrimitive()))
        raiseTypeError(operation, lhs, rhs);

    return std::forward_as_tuple(lhs, rhs);
}

void Vm::add()
{
    auto [lhs, rhs] = operands();
    if (lhs.isPrimitive() && rhs.isPrimitive())
        primitiveBinary(lhs, rhs, [](auto a, auto b) { return a + b; });
    else if (lhs.isString() && rhs.isString())
    {
        std::string str;
        str.reserve(
            static_cast<DzString*>(lhs.o)->data.size() +
            static_cast<DzString*>(rhs.o)->data.size());
        str.append(static_cast<DzString*>(lhs.o)->data);
        str.append(static_cast<DzString*>(rhs.o)->data);
        lhs.set(interning.make(std::move(str)));
    }
    else
        raiseTypeError("+", lhs, rhs);
}

void Vm::assertion()
{
    if (!stack.pop())
        raise<AssertionError>("assertion failed");
}

void Vm::bitAnd()
{
    auto [lhs, rhs] = bitwiseOperands("&");
    bitwiseBinary(lhs, rhs, [](auto a, auto b) { return a & b; });
}

void Vm::bitAsr()
{
    auto [lhs, rhs] = bitwiseOperands(">>");
    bitwiseBinary(lhs, rhs, [](auto a, auto b) { return a >> b; });
}

void Vm::bitComplement()
{
    auto& value = stack[0];
    switch (value.type)
    {
    case DzValue::Type::Int:  value.set(~value.i); break;
    case DzValue::Type::Bool: value.set(~static_cast<dzint>(value.b)); break;

    default:
        raiseTypeError("~", value);
        break;
    }
}

void Vm::bitLsl()
{
    auto [lhs, rhs] = bitwiseOperands("<<");
    bitwiseBinary(lhs, rhs, [](auto a, auto b) { return a << b; });
}

void Vm::bitLsr()
{
    auto [lhs, rhs] = bitwiseOperands(">>>");
    bitwiseBinary(lhs, rhs, [](auto a, auto b)
    { 
        using T = decltype(a);
        using U = std::make_unsigned_t<T>;

        return static_cast<T>(static_cast<U>(a) >> b);
    });
}

void Vm::bitOr()
{
    auto [lhs, rhs] = bitwiseOperands("|");
    bitwiseBinary(lhs, rhs, [](auto a, auto b) { return a | b; });
}

void Vm::bitXor()
{
    auto [lhs, rhs] = bitwiseOperands("^");
    bitwiseBinary(lhs, rhs, [](auto a, auto b) { return a ^ b; });
}

void Vm::constant()
{
    stack.push(chunk.constants[read<u8>()]);
}

void Vm::constantExt()
{
    stack.push(chunk.constants[read<u16>()]);
}

void Vm::discard()
{
    stack.pop();
}

void Vm::divide()
{
    auto [lhs, rhs] = primitiveOperands("/");
    primitiveBinary(lhs, rhs, [this](auto a, auto b)
    {
        if (b == static_cast<dzfloat>(0))
            raise<ZeroDivsionError>("division by zero");
        return static_cast<dzfloat>(a) / static_cast<dzfloat>(b);
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
    if (lhs.isPrimitive() && rhs.isPrimitive())
        primitiveBinary(lhs, rhs, [](auto a, auto b) { return a == b; });
    else
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
    case DzValue::Type::Int:   value.set(-value.i); break;
    case DzValue::Type::Float: value.set(-value.f); break;
    case DzValue::Type::Bool:  value.set(-static_cast<dzint>(value.b)); break;

    default:
        raise<TypeError>("bad operand type for '-': '{}'", value.typeName());
        break;
    }
}

void Vm::not_()
{
    auto& value = stack[0];
    value.set(!static_cast<bool>(value));
}

void Vm::notEqual()
{
    auto [lhs, rhs] = operands();
    if (lhs.isPrimitive() && rhs.isPrimitive())
        primitiveBinary(lhs, rhs, [](auto a, auto b) { return a != b; });
    else
        lhs.set(lhs != rhs);
}

void Vm::power()
{
    auto [lhs, rhs] = primitiveOperands("**");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return std::pow(a, b); });
}

void Vm::print()
{
    shell::print("{}\n", stack.pop());
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
