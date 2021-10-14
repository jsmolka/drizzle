#include "vm.h"

#include <shell/punning.h>

#include "compiler.h"
#include "dzstring.h"
#include "errors.h"
#include "format.h"

void Vm::interpret(const Tokens& tokens)
{
    Compiler compiler(interning, Compiler::Type::Main);
    auto main = compiler.compile(tokens.begin());

    frames.push_back({ main, main->chunk.code.data(), 0 });
    frame = &frames.back();

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
        case Opcode::False: pushFalse(); break;
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
        case Opcode::Null: pushNull(); break;
        case Opcode::Pop: pop(); break;
        case Opcode::PopMultiple: popMultiple(); break;
        case Opcode::PopMultipleExt: popMultipleExt(); break;
        case Opcode::Power: power(); break;
        case Opcode::Print: print(); break;
        case Opcode::StoreVariable: storeVariable(); break;
        case Opcode::StoreVariableExt: storeVariableExt(); break;
        case Opcode::Subtract: subtract(); break;
        case Opcode::True: pushTrue(); break;

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

    auto value = shell::read<Integral>(frame->pc, 0);
    frame->pc += sizeof(Integral);
    return value;
}

template<typename Error, typename ...Args>
void Vm::raise(std::string_view message, Args&& ...args)
{
    static_assert(std::is_base_of_v<RuntimeError, Error>);

    auto index = frame->pc - frame->function->chunk.code.data();
    auto line = frame->function->chunk.line(index);

    throw Error(line, message, std::forward<Args>(args)...);
}

template<template<typename T> typename Promote, typename Handler>
void Vm::unary(std::string_view operation, Handler handler)
{
    static_assert(int(DzValue::Type::LastEnumValue) == 5);

    auto& value = stack.top();

    auto promote = [handler, &value = value](auto a)
    {
        using A = decltype(a);

        if constexpr (is_dz_primitive_v<A>)
            return handler(value, static_cast<Promote<A>>(a));
        else
            return handler(value, a);
    };

    #define DZ_EVAL(a) if (promote(a)) return; else break

    switch (value.type)
    {
    case DzValue::Type::Bool:   DZ_EVAL(value.b);
    case DzValue::Type::Int:    DZ_EVAL(value.i);
    case DzValue::Type::Float:  DZ_EVAL(value.f);
    case DzValue::Type::Null:   DZ_EVAL(DzNull{});
    case DzValue::Type::Object: DZ_EVAL(value.o);

    default:
        SHELL_UNREACHABLE;
        break;
    }

    #undef DZ_EVAL

    raise<TypeError>("bad operand type for '{}': '{}'", operation, value.typeName());
}

template<template<typename T, typename U> typename Promote, typename Handler>
void Vm::binary(std::string_view operation, Handler handler)
{
    static_assert(int(DzValue::Type::LastEnumValue) == 5);

    auto  rhs = stack.popValue();
    auto& lhs = stack.top();

    auto promote = [handler, &lhs = lhs](auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_primitive_v<A, B>)
            return handler(lhs, static_cast<Promote<A, B>>(a), static_cast<Promote<A, B>>(b));
        else
            return handler(lhs, a, b);
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

void Vm::add()
{
    binary("+", [this](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = a + b;
            return true;
        }
        if constexpr (is_dz_object_v<A, B>)
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

        if constexpr (is_dz_bool_v<A, B>)
        {
            dst = static_cast<dzbool>(a & b);
            return true;
        }
        if constexpr (is_dz_int_v<A, B>)
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

        if constexpr (is_dz_int_v<A, B>)
        {
            dst = a >> b;
            return true;
        }
        return false;
    });
}

void Vm::bitComplement()
{
    unary("~", [](DzValue& dst, auto a)
    {
        using A = decltype(a);

        if constexpr (is_dz_int_v<A>)
        {
            dst = ~a;
            return true;
        }
        return false;
    });
}

void Vm::bitLsl()
{
    binary("<<", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B>)
        {
            dst = a << b;
            return true;
        }
        return false;
    });
}

void Vm::bitLsr()
{
    binary(">>>", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B>)
        {
            dst = static_cast<dzint>(static_cast<std::make_unsigned_t<dzint>>(a) >> b);
            return true;
        }
        return false;
    });
}

void Vm::bitOr()
{
    binary<promote_lax_t>("|", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_bool_v<A, B>)
        {
            dst = static_cast<dzbool>(a | b);
            return true;
        }
        if constexpr (is_dz_int_v<A, B>)
        {
            dst = a | b;
            return true;
        }
        return false;
    });
}

void Vm::bitXor()
{
    binary<promote_lax_t>("^", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_bool_v<A, B>)
        {
            dst = static_cast<dzbool>(a ^ b);
            return true;
        }
        if constexpr (is_dz_int_v<A, B>)
        {
            dst = a ^ b;
            return true;
        }
        return false;
    });
}

void Vm::constant()
{
    auto index = read<u8>();
    stack.push(frame->function->chunk.constants[index]);
}

void Vm::constantExt()
{
    auto index = read<u16>();
    stack.push(frame->function->chunk.constants[index]);
}

void Vm::divide()
{
    binary("/", [this](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            if (b == static_cast<B>(0))
                raise<ZeroDivsionError>("division by zero");

            dst = static_cast<dzfloat>(a) / static_cast<dzfloat>(b);
            return true;
        }
        return false;
    });
}

void Vm::divideInt()
{
    binary("//", [this](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            if (b == static_cast<B>(0))
                raise<ZeroDivsionError>("integer division by zero");

            if constexpr (is_dz_int_v<A, B>)
                dst = a / b;
            else
                dst = std::floor(a / b);
            return true;
        }
        return false;
    });
}

void Vm::equal()
{
    binary("==", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_primitive_v<A, B>)
        {
            dst = a == b;
            return true;
        }
        if constexpr (is_dz_object_v<A, B>)
        {
            dst = *a == *b;
            return true;
        }
        dst = is_dz_null_v<A, B>;
        return true;
    });
}

void Vm::greater()
{
    binary(">", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = a > b;
            return true;
        }
        return false;
    });
}

void Vm::greaterEqual()
{
    binary(">=", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = a >= b;
            return true;
        }
        return false;
    });
}

void Vm::jump()
{
    frame->pc += read<s16>();
}

void Vm::jumpFalse()
{
    auto offset = read<s16>();
    if (!stack.top())
        frame->pc += offset;
}

void Vm::jumpFalsePop()
{
    auto offset = read<s16>();
    if (!stack.popValue())
        frame->pc += offset;
}

void Vm::jumpTrue()
{
    auto offset = read<s16>();
    if (stack.top())
        frame->pc += offset;
}

void Vm::less()
{
    binary("<", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = a < b;
            return true;
        }
        return false;
    });
}

void Vm::lessEqual()
{
    binary("<=", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = a <= b;
            return true;
        }
        return false;
    });
}

void Vm::loadVariable()
{
    auto index = read<u8>();
    stack.push(stack[frame->sp + index]);
}

void Vm::loadVariableExt()
{
    auto index = read<u16>();
    stack.push(stack[frame->sp + index]);
}

void Vm::modulo()
{
    binary("%", [this](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            if (b == static_cast<B>(0))
                raise<ZeroDivsionError>("modulo by zero");

            if constexpr (is_dz_int_v<A, B>)
                dst = a % b;
            else
                dst = std::fmod(a, b);
            return true;
        }
        return false;
    });
}

void Vm::multiply()
{
    binary("*", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = a * b;
            return true;
        }
        return false;
    });
}

void Vm::negate()
{
    unary("-", [](DzValue& dst, auto a)
    {
        using A = decltype(a);

        if constexpr (is_dz_int_v<A> || is_dz_float_v<A>)
        {
            dst = -a;
            return true;
        }
        return false;
    });
}

void Vm::not_()
{
    stack.top() = !stack.top();
}

void Vm::notEqual()
{
    binary("!=", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_primitive_v<A, B>)
        {
            dst = a != b;
            return true;
        }
        if constexpr (is_dz_object_v<A, B>)
        {
            dst = *a != *b;
            return true;
        }
        dst = !is_dz_null_v<A, B>;
        return true;
    });
}

void Vm::pop()
{
    stack.pop();
}

void Vm::popMultiple()
{
    auto count = read<u8>();
    stack.pop(count);
}

void Vm::popMultipleExt()
{
    auto count = read<u16>();
    stack.pop(count);
}

void Vm::power()
{
    binary("**", [](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = std::pow(a, b);
            return true;
        }
        return false;
    });
}

void Vm::print()
{
    shell::print("{}\n", stack.popValue());
}

void Vm::pushFalse()
{
    stack.push(false);
}

void Vm::pushNull()
{
    stack.push({});
}

void Vm::pushTrue()
{
    stack.push(true);
}

void Vm::storeVariable()
{
    auto index = read<u8>();
    stack[frame->sp + index] = stack.top();
}

void Vm::storeVariableExt()
{
    auto index = read<u16>();
    stack[frame->sp + index] = stack.top();
}

void Vm::subtract()
{
    binary("-", [this](DzValue& dst, auto a, auto b)
    {
        using A = decltype(a);
        using B = decltype(b);

        if constexpr (is_dz_int_v<A, B> || is_dz_float_v<A, B>)
        {
            dst = a - b;
            return true;
        }
        return false;
    });
}
