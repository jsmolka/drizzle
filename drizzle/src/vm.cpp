#include "vm.h"

#include <shell/punning.h>

#include "compiler.h"
#include "dzstring.h"
#include "errors.h"
#include "format.h"

void Vm::interpret(const Tokens& tokens)
{
    // Todo: free
    auto main = new DzFunction();
    Compiler compiler(interning);
    compiler.compile(tokens, *main);

    stack.push(main);
    call(new DzClosure(*main), 0);

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
        case Opcode::Call: callValue(); break;
        case Opcode::Closure: closure<u8>(); break;
        case Opcode::ClosureExt: closure<u16>(); break;
        case Opcode::Constant: constant<u8>(); break;
        case Opcode::ConstantExt: constant<u16>(); break;
        case Opcode::Divide: divide(); break;
        case Opcode::DivideInt: divideInt(); break;
        case Opcode::Equal: equal(); break;
        case Opcode::False: pushFalse(); break;
        case Opcode::Greater: greater(); break;
        case Opcode::GreaterEqual: greaterEqual(); break;
        case Opcode::Jump: jump(); break;
        case Opcode::JumpFalse: jumpFalse(); break;
        case Opcode::JumpFalsePop: jumpFalsePop(); break;
        case Opcode::JumpTrue: jumpTrue(); break;
        case Opcode::Less: less(); break;
        case Opcode::LessEqual: lessEqual(); break;
        case Opcode::LoadUpvalue: loadUpvalue<u8>(); break;
        case Opcode::LoadUpvalueExt: loadUpvalue<u16>(); break;
        case Opcode::LoadVariable: loadVariable<u8>(); break;
        case Opcode::LoadVariableExt: loadVariable<u16>(); break;
        case Opcode::Modulo: modulo(); break;
        case Opcode::Multiply: multiply(); break;
        case Opcode::Negate: negate(); break;
        case Opcode::Not: not_(); break;
        case Opcode::NotEqual: notEqual(); break;
        case Opcode::Null: pushNull(); break;
        case Opcode::Pop: pop(); break;
        case Opcode::PopMultiple: popMultiple<u8>(); break;
        case Opcode::PopMultipleExt: popMultiple<u16>(); break;
        case Opcode::Power: power(); break;
        case Opcode::Print: print(); break;
        case Opcode::Return: if (return_()) return; break;
        case Opcode::StoreUpvalue: storeUpvalue<u8>(); break;
        case Opcode::StoreUpvalueExt: storeUpvalue<u16>(); break;
        case Opcode::StoreVariable: storeVariable<u8>(); break;
        case Opcode::StoreVariableExt: storeVariable<u16>(); break;
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

    auto index = frame->pc - frame->closure->function.chunk.code.data();
    auto line = frame->closure->function.chunk.line(index);

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

DzUpvalue* Vm::capture(std::size_t index)
{
    return new DzUpvalue(stack[frame->sp + index]);
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

void Vm::call(DzClosure* closure, u8 argc)
{
    if (argc != closure->function.arity)
        raise<RuntimeError>("expected {} argument(s) but got {}", closure->function.arity, argc);

    frames.push_back({ closure, closure->function.chunk.code.data(), stack.size() - argc });
    frame = &frames.back();
}

void Vm::callValue()
{
    auto argc = read<u8>();
    auto& callee = stack.peek(argc);
    if (callee.type != DzValue::Type::Object)
        raise<RuntimeError>("tried calling {}", callee.typeName());
    if (callee.o->type != DzObject::Type::Closure)
        raise<RuntimeError>("tried calling {}", callee.typeName());

    auto closure = static_cast<DzClosure*>(callee.o);
    call(closure, argc);
}

template<typename Integral>
void Vm::closure()
{
    auto index = read<Integral>();
    auto function = static_cast<DzFunction*>(frame->closure->function.chunk.constants[index].o);
    auto closure = new DzClosure(*function);
    stack.push(closure);

    for (auto& upvalue : closure->upvalues)
    {
        auto is_local = read<u8>();
        auto index = read<u8>();

        if (is_local)
            upvalue = capture(index);
        else
            upvalue = frame->closure->upvalues[index];
    }

}

template<typename Integral>
void Vm::constant()
{
    auto index = read<Integral>();
    stack.push(frame->closure->function.chunk.constants[index]);
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

template<typename Integral>
void Vm::loadUpvalue()
{
    auto index = read<Integral>();
    stack.push(frame->closure->upvalues[index]->value);
}

template<typename Integral>
void Vm::loadVariable()
{
    auto index = read<Integral>();
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

template<typename Integral>
void Vm::popMultiple()
{
    auto count = read<Integral>();
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

bool Vm::return_()
{
    auto result = stack.popValue();
    auto sp = frames.back().sp;

    frames.pop_back();
    if (frames.empty())
        return true;

    frame = &frames.back();
    stack.pop(stack.size() - sp);
    stack.top() = result;

    return false;
}

template<typename Integral>
void Vm::storeUpvalue()
{
    auto index = read<Integral>();
    frame->closure->upvalues[index]->value = stack.top();
}

template<typename Integral>
void Vm::storeVariable()
{
    auto index = read<Integral>();
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
