#include "vm.h"

#include <shell/punning.h>
#include <shell/traits.h>

#include "compiler.h"
#include "dzstring.h"
#include "errors.h"
#include "format.h"

template<typename T, typename... Ts>
inline constexpr auto is_same_v = shell::is_same_v<T, shell::unqualified_t<Ts>...>;

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
        case Opcode::Divide: divide(); break;
        case Opcode::DivideInt: divideInt(); break;
        case Opcode::Equal: equal(); break;
        case Opcode::Exit: return;
        case Opcode::False: valueFalse(); break;
        case Opcode::Greater: greater(); break;
        case Opcode::GreaterEqual: greaterEqual(); break;
        case Opcode::Jump: jump(); break;
        case Opcode::JumpFalse: jumpFalse(); break;
        case Opcode::JumpFalsePop: jumpFalsePop(); break;
        case Opcode::JumpTrue: jumpTrue(); break;
        case Opcode::Less: less(); break;
        case Opcode::LessEqual: lessEqual(); break;
        case Opcode::LoadVariable: loadVariable(); break;
        case Opcode::LoadVariableExt: loadVariableExt(); break;
        case Opcode::Modulo: modulo(); break;
        case Opcode::Multiply: multiply(); break;
        case Opcode::Negate: negate(); break;
        case Opcode::Not: not_(); break;
        case Opcode::NotEqual: notEqual(); break;
        case Opcode::Null: valueNull(); break;
        case Opcode::Pop: pop(); break;
        case Opcode::PopMultiple: popMultiple(); break;
        case Opcode::PopMultipleExt: popMultipleExt(); break;
        case Opcode::Power: power(); break;
        case Opcode::Print: print(); break;
        case Opcode::StoreVariable: storeVariable(); break;
        case Opcode::StoreVariableExt: storeVariableExt(); break;
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

template<template<typename T, typename U> typename Promote, typename Callback>
void Vm::binary(std::string_view operation, Callback callback)
{
    static_assert(int(DzValue::Type::LastEnumValue) == 5);

    auto  rhs = stack.popValue();
    auto& lhs = stack.peek(0);

    auto promote = [callback, &lhs = lhs](auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_primitive_v<A> && is_dz_primitive_v<B>)
            return callback(lhs, static_cast<Promote<A, B>>(a), static_cast<Promote<A, B>>(b));
        else
            return callback(lhs, a, b);
    };

    #define DZ_EVAL(a, b) if (promote(a, b)) return; else break
    #define DZ_HASH(a, b) (int(DzValue::Type::LastEnumValue) * int(a) + int(b))

    switch (DZ_HASH(lhs.type, rhs.type))
    {
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Bool  ): DZ_EVAL(lhs.b, rhs.b);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Int   ): DZ_EVAL(lhs.b, rhs.i);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Float ): DZ_EVAL(lhs.b, rhs.f);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Null  ): DZ_EVAL(lhs.b, DzNull{});
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Object): DZ_EVAL(lhs.b, rhs.o);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Bool  ): DZ_EVAL(lhs.i, rhs.b);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Int   ): DZ_EVAL(lhs.i, rhs.i);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Float ): DZ_EVAL(lhs.i, rhs.f);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Null  ): DZ_EVAL(lhs.i, DzNull{});
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Object): DZ_EVAL(lhs.i, rhs.o);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Bool  ): DZ_EVAL(lhs.f, rhs.b);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Int   ): DZ_EVAL(lhs.f, rhs.i);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Float ): DZ_EVAL(lhs.f, rhs.f);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Null  ): DZ_EVAL(lhs.f, DzNull{});
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Object): DZ_EVAL(lhs.f, rhs.o);
    case DZ_HASH(DzValue::Type::Null,   DzValue::Type::Bool  ): DZ_EVAL(DzNull{}, rhs.b);
    case DZ_HASH(DzValue::Type::Null,   DzValue::Type::Int   ): DZ_EVAL(DzNull{}, rhs.i);
    case DZ_HASH(DzValue::Type::Null,   DzValue::Type::Float ): DZ_EVAL(DzNull{}, rhs.f);
    case DZ_HASH(DzValue::Type::Null,   DzValue::Type::Null  ): DZ_EVAL(DzNull{}, DzNull{});
    case DZ_HASH(DzValue::Type::Null,   DzValue::Type::Object): DZ_EVAL(DzNull{}, rhs.o);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Bool  ): DZ_EVAL(lhs.o, rhs.b);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Int   ): DZ_EVAL(lhs.o, rhs.i);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Float ): DZ_EVAL(lhs.o, rhs.f);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Null  ): DZ_EVAL(lhs.o, DzNull{});
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Object): DZ_EVAL(lhs.o, rhs.o);

    default:
        SHELL_UNREACHABLE;
        break;
    }

    #undef DZ_HASH
    #undef DZ_EVAL

    raise<TypeError>("bad operand types for '{}': '{}' and '{}'", operation, lhs.typeName(), rhs.typeName());
}

template<typename Operation>
void Vm::bitwiseBinary(DzValue& lhs, const DzValue& rhs, Operation operation)
{
    auto promote = [operation](auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);
        using P = promote_t<A, B>;

        return operation(static_cast<P>(a), static_cast<P>(b));
    };

    #define HASH(a, b) ((int(a) << 1) | int(b))

    switch (HASH(lhs.type, rhs.type))
    {
    case HASH(DzValue::Type::Int,   DzValue::Type::Int ): lhs = promote(lhs.i, rhs.i); break;
    case HASH(DzValue::Type::Int,   DzValue::Type::Bool): lhs = promote(lhs.i, rhs.b); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Int ): lhs = promote(lhs.b, rhs.i); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Bool): lhs = promote(lhs.b, rhs.b); break;

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
        using P = promote_t<A, B>;

        return operation(static_cast<P>(a), static_cast<P>(b));
    };

    #define HASH(a, b) ((int(a) << 2) | int(b))

    switch (HASH(lhs.type, rhs.type))
    {
    case HASH(DzValue::Type::Int,   DzValue::Type::Int  ): lhs = promote(lhs.i, rhs.i); break;
    case HASH(DzValue::Type::Int,   DzValue::Type::Float): lhs = promote(lhs.i, rhs.f); break;
    case HASH(DzValue::Type::Int,   DzValue::Type::Bool ): lhs = promote(lhs.i, rhs.b); break;
    case HASH(DzValue::Type::Float, DzValue::Type::Int  ): lhs = promote(lhs.f, rhs.i); break;
    case HASH(DzValue::Type::Float, DzValue::Type::Float): lhs = promote(lhs.f, rhs.f); break;
    case HASH(DzValue::Type::Float, DzValue::Type::Bool ): lhs = promote(lhs.f, rhs.b); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Int  ): lhs = promote(lhs.b, rhs.i); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Float): lhs = promote(lhs.b, rhs.f); break;
    case HASH(DzValue::Type::Bool,  DzValue::Type::Bool ): lhs = promote(lhs.b, rhs.b); break;

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
    auto  rhs = stack.popValue();
    auto& lhs = stack.peek(0);

    return std::forward_as_tuple(lhs, rhs);
}

std::tuple<DzValue&, DzValue> Vm::bitwiseOperands(std::string_view operation)
{
    auto [lhs, rhs] = operands();

    //if (!(lhs.isBitwise() && rhs.isBitwise()))
    //    raiseTypeError(operation, lhs, rhs);

    return std::forward_as_tuple(lhs, rhs);
}

std::tuple<DzValue&, DzValue> Vm::primitiveOperands(std::string_view operation)
{
    auto [lhs, rhs] = operands();
    
    //if (!(lhs.isPrimitive() && rhs.isPrimitive()))
    //    raiseTypeError(operation, lhs, rhs);

    return std::forward_as_tuple(lhs, rhs);
}

void Vm::add()
{
    binary("+", [this](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_same_v<dzint, A, B> || is_same_v<dzfloat, A, B>)
        {
            dst = a + b;
            return true;
        }
        if constexpr (is_same_v<DzObject, A, B>)
        {
            if (a->type == DzObject::Type::String && b->type == DzObject::Type::String)
            {
                auto aa = static_cast<DzString*>(a);
                auto bb = static_cast<DzString*>(b);

                dst = interning.make(aa->data + bb->data);
                return true;
            }
        }
        return false;
    });
}

void Vm::assertion()
{
    if (!stack.popValue())
        raise<AssertionError>("assertion failed");
}

void Vm::bitAnd()
{
    binary<promote_lax_t>("&", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_same_v<dzbool, A, B>)
        {
            dst = static_cast<dzbool>(a & b);
            return true;
        }
        if constexpr (is_same_v<dzint, A, B>)
        {
            dst = a & b;
            return true;
        }
        return false;
    });
}

void Vm::bitAsr()
{
    binary(">>", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_same_v<dzint, A, B>)
        {
            dst = a >> b;
            return true;
        }
        return false;
    });
}

void Vm::bitComplement()
{
    auto& value = stack.peek(0);
    switch (value.type)
    {
    case DzValue::Type::Int:  value = ~value.i; break;
    case DzValue::Type::Bool: value = ~static_cast<dzint>(value.b); break;

    default:
        raiseTypeError("~", value);
        break;
    }
}

void Vm::bitLsl()
{
    binary("<<", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_same_v<dzint, A, B>)
        {
            dst = a << b;
            return true;
        }
        return false;
    });
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
    //auto [lhs, rhs] = operands();
    //if (lhs.isPrimitive() && rhs.isPrimitive())
    //    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a == b; });
    //else
    //    lhs = lhs == rhs;
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

void Vm::jump()
{
    ip += read<s16>();
}

void Vm::jumpFalse()
{
    auto offset = read<s16>();
    if (!stack.peek(0))
        ip += offset;
}

void Vm::jumpFalsePop()
{
    auto offset = read<s16>();
    if (!stack.popValue())
        ip += offset;
}

void Vm::jumpTrue()
{
    auto offset = read<s16>();
    if (stack.peek(0))
        ip += offset;
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

void Vm::loadVariable()
{
    auto index = read<u8>();
    stack.push(stack[index]);
}

void Vm::loadVariableExt()
{
    auto index = read<u16>();
    stack.push(stack[index]);
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
    auto& value = stack.peek(0);
    switch (value.type)
    {
    case DzValue::Type::Int:   value = -value.i; break;
    case DzValue::Type::Float: value = -value.f; break;
    case DzValue::Type::Bool:  value = -static_cast<dzint>(value.b); break;

    default:
        raise<TypeError>("bad operand type for '-': '{}'", value.typeName());
        break;
    }
}

void Vm::not_()
{
    auto& value = stack.peek(0);
    value = !static_cast<bool>(value);
}

void Vm::notEqual()
{
    //auto [lhs, rhs] = operands();
    //if (lhs.isPrimitive() && rhs.isPrimitive())
    //    primitiveBinary(lhs, rhs, [](auto a, auto b) { return a != b; });
    //else
    //    lhs = lhs != rhs;
}

void Vm::pop()
{
    stack.pop();
}

void Vm::popMultiple()
{
    stack.pop(read<u8>());
}

void Vm::popMultipleExt()
{
    stack.pop(read<u16>());
}

void Vm::power()
{
    auto [lhs, rhs] = primitiveOperands("**");
    primitiveBinary(lhs, rhs, [](auto a, auto b) { return std::pow(a, b); });
}

void Vm::print()
{
    shell::print("{}\n", stack.popValue());
}

void Vm::storeVariable()
{
    auto index = read<u8>();
    stack[index] = stack.peek(0);
}

void Vm::storeVariableExt()
{
    auto index = read<u16>();
    stack[index] = stack.peek(0);
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
