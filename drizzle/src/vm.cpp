#include "vm.h"

#include "dzbuiltin.h"
#include "dznull.h"
#include "dzstring.h"
#include "opcode.h"

void Vm::interpret(DzFunction* function) {
  static_assert(int(Opcode::LastEnumValue) == 44);

  frames.push(Frame{
    .function = function,
    .pc = function->chunk.code.data(),
    .sp = 0
  });

  while (true) {
    switch (static_cast<Opcode>(read<u8>())) {
      case Opcode::Add: add(); break;
      case Opcode::BitwiseAnd: bitwiseAnd(); break;
      case Opcode::BitwiseAsr: bitwiseAsr(); break;
      case Opcode::BitwiseComplement: bitwiseComplement(); break;
      case Opcode::BitwiseLsr: bitwiseLsr(); break;
      case Opcode::BitwiseLsl: bitwiseLsl(); break;
      case Opcode::BitwiseOr: bitwiseOr(); break;
      case Opcode::BitwiseXor: bitwiseXor(); break;
      case Opcode::Call: call(); break;
      case Opcode::Constant: constant<u8>(); break;
      case Opcode::ConstantExt: constant<u16>(); break;
      case Opcode::Divide: divide(); break;
      case Opcode::DivideInt: divideInt(); break;
      case Opcode::Equal: equal(); break;
      case Opcode::False: false_(); break;
      case Opcode::Greater: greater(); break;
      case Opcode::GreaterEqual: greaterEqual(); break;
      case Opcode::Jump: jump(); break;
      case Opcode::JumpFalse: jumpFalse(); break;
      case Opcode::JumpFalsePop: jumpFalsePop(); break;
      case Opcode::JumpTrue: jumpTrue(); break;
      case Opcode::Less: less(); break;
      case Opcode::LessEqual: lessEqual(); break;
      case Opcode::Load: load<u8>(); break;
      case Opcode::LoadExt: load<u16>(); break;
      case Opcode::LoadAbsolute: loadAbsolute<u8>(); break;
      case Opcode::LoadAbsoluteExt: loadAbsolute<u16>(); break;
      case Opcode::Modulo: modulo(); break;
      case Opcode::Multiply: multiply(); break;
      case Opcode::Negate: negate(); break;
      case Opcode::Not: not_(); break;
      case Opcode::NotEqual: notEqual(); break;
      case Opcode::Null: null(); break;
      case Opcode::Pop: pop(); break;
      case Opcode::PopMultiple: popMultiple<u8>(); break;
      case Opcode::PopMultipleExt: popMultiple<u16>(); break;
      case Opcode::Power: power(); break;
      case Opcode::Return: if (return_()) { return; } break;
      case Opcode::Store: store<u8>(); break;
      case Opcode::StoreExt: store<u16>(); break;
      case Opcode::StoreAbsolute: storeAbsolute<u8>(); break;
      case Opcode::StoreAbsoluteExt: storeAbsolute<u16>(); break;
      case Opcode::Subtract: subtract(); break;
      case Opcode::True: true_(); break;
      default:
        SH_UNREACHABLE;
        break;
    }
  }
}

auto Vm::frame() -> Frame& {
  return frames.top();
}

template<std::integral Integral>
auto Vm::read() -> Integral {
  const auto value = sh::cast<Integral>(*frame().pc);
  frame().pc += sizeof(Integral);
  return value;
}

template<typename... Args>
void Vm::raise(std::string_view format, Args&&... args) {
  const auto line = frame().function->chunk.line(
    frame().pc -
    frame().function->chunk.code.data());
  throw RuntimeError(Location{.line = line}, format, std::forward<Args>(args)...);
}

template<template<typename T> typename Promote, UnaryHandler Handler>
void Vm::unary(std::string_view operation, Handler handler) {
  static_assert(int(DzValue::Type::LastEnumValue) == 4);

  auto& value = stack.top();

  #define DZ_EVAL(a)                       \
  {                                        \
    using A = decltype(a);                 \
    if constexpr (dz_primitive<A>) {       \
      if (handler(value, Promote<A>(a))) { \
        return;                            \
      } else {                             \
        break;                             \
      }                                    \
    } else {                               \
      if (handler(value, a)) {             \
        return;                            \
      } else {                             \
        break;                             \
      }                                    \
    }                                      \
    break;                                 \
  }

  switch (value.type) {
    case DzValue::Type::Bool:   DZ_EVAL(value.b);
    case DzValue::Type::Int:    DZ_EVAL(value.i);
    case DzValue::Type::Float:  DZ_EVAL(value.f);
    case DzValue::Type::Object: DZ_EVAL(value.o);
    default:
      SH_UNREACHABLE;
      break;
  }

  #undef DZ_EVAL

  raise("bad operand type for '{}': '{}'", operation, value.name());
}

template<template<typename T, typename U> typename Promote, BinaryHandler Handler>
void Vm::binary(std::string_view operation, Handler handler) {
  static_assert(int(DzValue::Type::LastEnumValue) == 4);

  auto  rhs = stack.pop_value();
  auto& lhs = stack.top();

  #define DZ_EVAL(a, b)                                       \
  {                                                           \
    using A = decltype(a);                                    \
    using B = decltype(b);                                    \
    if constexpr (dz_primitive<A, B>) {                       \
      if (handler(lhs, Promote<A, B>(a), Promote<A, B>(b))) { \
        return;                                               \
      } else {                                                \
        break;                                                \
      }                                                       \
    } else {                                                  \
      if (handler(lhs, a, b)) {                               \
        return;                                               \
      } else {                                                \
        break;                                                \
      }                                                       \
    }                                                         \
    break;                                                    \
  }

  #define DZ_HASH(a, b) (int(DzValue::Type::LastEnumValue) * int(a) + int(b))

  switch (DZ_HASH(lhs.type, rhs.type)) {
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Bool  ): DZ_EVAL(lhs.b, rhs.b);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Int   ): DZ_EVAL(lhs.b, rhs.i);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Float ): DZ_EVAL(lhs.b, rhs.f);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Object): DZ_EVAL(lhs.b, rhs.o);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Bool  ): DZ_EVAL(lhs.i, rhs.b);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Int   ): DZ_EVAL(lhs.i, rhs.i);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Float ): DZ_EVAL(lhs.i, rhs.f);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Object): DZ_EVAL(lhs.i, rhs.o);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Bool  ): DZ_EVAL(lhs.f, rhs.b);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Int   ): DZ_EVAL(lhs.f, rhs.i);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Float ): DZ_EVAL(lhs.f, rhs.f);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Object): DZ_EVAL(lhs.f, rhs.o);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Bool  ): DZ_EVAL(lhs.o, rhs.b);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Int   ): DZ_EVAL(lhs.o, rhs.i);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Float ): DZ_EVAL(lhs.o, rhs.f);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Object): DZ_EVAL(lhs.o, rhs.o);
    default:
      SH_UNREACHABLE;
      break;
  }

  #undef DZ_HASH
  #undef DZ_EVAL

  raise("bad operand types for '{}': '{}' and '{}'", operation, lhs.name(), rhs.name());
}

void Vm::add() {
  binary("+", [this]<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a + b;
      return true;
    } else if constexpr (dz_object<A, B>) {
      if (a->type == DzObject::Type::String && b->type == DzObject::Type::String) {
        auto str_a = static_cast<DzString*>(a);
        auto str_b = static_cast<DzString*>(b);
        dst = new DzString(str_a->data + str_b->data);
        return true;
      }
    }
    return false;
  });
}

void Vm::bitwiseAnd() {
  binary<promote_lax_t>("&", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_bool<A, B>) {
      dst = static_cast<dzbool>(a & b);
      return true;
    } else if constexpr (dz_int<A, B>) {
      dst = a & b;
      return true;
    }
    return false;
  });
}

void Vm::bitwiseAsr() {
  binary(">>", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B>) {
      dst = a >> b;
      return true;
    }
    return false;
  });
}

void Vm::bitwiseComplement() {
  unary("~", []<typename A>(DzValue& dst, const A& a) {
    if constexpr (dz_int<A>) {
      dst = ~a;
      return true;
    }
    return false;
  });
}

void Vm::bitwiseLsl() {
  binary("<<", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B>) {
      dst = a << b;
      return true;
    }
    return false;
  });
}

void Vm::bitwiseLsr() {
  binary(">>>", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B>) {
      dst = static_cast<dzint>(static_cast<std::make_unsigned_t<dzint>>(a) >> b);
      return true;
    }
    return false;
  });
}

void Vm::bitwiseOr() {
  binary<promote_lax_t>("|", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_bool<A, B>) {
      dst = static_cast<dzbool>(a | b);
      return true;
    } else if constexpr (dz_int<A, B>) {
      dst = a | b;
      return true;
    }
    return false;
  });
}

void Vm::bitwiseXor() {
  binary<promote_lax_t>("^", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_bool<A, B>) {
      dst = static_cast<dzbool>(a ^ b);
      return true;
    } else if constexpr (dz_int<A, B>) {
      dst = a ^ b;
      return true;
    }
    return false;
  });
}

void Vm::call() {
  const auto argc = read<u8>();
  const auto& value = stack.peek(argc);
  if (value.type == DzValue::Type::Object) {
    switch (value.o->type) {
      case DzObject::Type::BuiltIn: {
        const auto builtin = static_cast<DzBuiltIn*>(value.o);
        if (builtin->arity && *builtin->arity != argc) {
          raise("expected {} argument(s) but got {}", *builtin->arity, argc);
        }
        builtin->callback(*this, argc);
        return;
      }

      case DzObject::Type::Function: {
        const auto function = static_cast<DzFunction*>(value.o);
        if (function->arity != argc) {
          raise("expected {} argument(s) but got {}", function->arity, argc);
        }
        constexpr auto kFunction = 1;
        frames.push(Frame{
          .function = function,
          .pc = function->chunk.code.data(),
          .sp = stack.size() - argc - kFunction
        });
        return;
      }
    }
  }
  raise("'{}' is not callable", value);
}

template<typename Integral>
void Vm::constant() {
  const auto index = read<Integral>();
  stack.push(frame().function->chunk.constants[index]);
}

void Vm::divide() {
  binary("/", [this]<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise("division by zero");
      }
      dst = static_cast<dzfloat>(a) / static_cast<dzfloat>(b);
      return true;
    }
    return false;
  });
}

void Vm::divideInt() {
  binary("//", [this]<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise("integer division by zero");
      }
      if constexpr (dz_int<A, B>) {
        dst = a / b;
      } else {
        dst = std::floor(a / b);
      }
      return true;
    }
    return false;
  });
}

void Vm::equal() {
  binary("==", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_primitive<A, B>) {
      dst = a == b;
      return true;
    } else if constexpr (dz_object<A, B>) {
      dst = *a == *b;
      return true;
    }
    return false;
  });
}

void Vm::false_() {
  stack.push(false);
}

void Vm::greater() {
  binary(">", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a > b;
      return true;
    }
    return false;
  });
}

void Vm::greaterEqual() {
  binary(">=", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a >= b;
      return true;
    }
    return false;
  });
}

void Vm::jump() {
  frame().pc += read<s16>();
}

void Vm::jumpFalse() {
  const auto offset = read<s16>();
  if (!stack.top()) {
    frame().pc += offset;
  }
}

void Vm::jumpFalsePop() {
  const auto offset = read<s16>();
  if (!stack.pop_value()) {
    frame().pc += offset;
  }
}

void Vm::jumpTrue() {
  const auto offset = read<s16>();
  if (stack.top()) {
    frame().pc += offset;
  }
}

void Vm::less() {
  binary("<", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a < b;
      return true;
    }
    return false;
  });
}

void Vm::lessEqual() {
  binary("<=", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a <= b;
      return true;
    }
    return false;
  });
}

template<typename Integral>
void Vm::load() {
  const auto index = read<Integral>();
  stack.push(stack[frame().sp + index]);
}

template<typename Integral>
void Vm::loadAbsolute() {
  const auto index = read<Integral>();
  stack.push(stack[index]);
}

void Vm::modulo() {
  binary("%", [this]<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise("modulo by zero");
      }
      if constexpr (dz_int<A, B>) {
        dst = a % b;
      } else {
        dst = std::fmod(a, b);
      }
      return true;
    }
    return false;
  });
}

void Vm::multiply() {
  binary("*", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a * b;
      return true;
    }
    return false;
  });
}

void Vm::negate() {
  unary("-", []<typename A>(DzValue& dst, const A& a) {
    if constexpr (dz_int<A> || dz_float<A>) {
      dst = -a;
      return true;
    }
    return false;
  });
}

void Vm::not_() {
  stack.top() = !stack.top();
}

void Vm::notEqual() {
  binary("!=", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_primitive<A, B>) {
      dst = a != b;
      return true;
    } else if constexpr (dz_object<A, B>) {
      dst = *a != *b;
      return true;
    }
    return false;
  });
}

void Vm::null() {
  stack.push(&::null);
}

void Vm::pop() {
  stack.pop();
}

template<typename Integral>
void Vm::popMultiple() {
  const auto count = read<Integral>();
  stack.pop(count);
}

void Vm::power() {
  binary("**", []<typename  A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = std::pow(a, b);
      return true;
    }
    return false;
  });
}

bool Vm::return_() {
  constexpr auto kFunction = 1;
  const auto result = stack.pop_value();
  const auto frame = frames.pop_value();
  if (!frames.empty()) {
    stack.pop(stack.size() - frame.sp - kFunction);
    stack.top() = result;
    return false;
  }
  assert(stack.empty());
  return true;
}

template<typename Integral>
void Vm::store() {
  const auto index = read<Integral>();
  stack[frame().sp + index] = stack.top();
}

template<typename Integral>
void Vm::storeAbsolute() {
  const auto index = read<Integral>();
  stack[index] = stack.top();
}

void Vm::subtract() {
  binary("-", []<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a - b;
      return true;
    }
    return false;
  });
}

void Vm::true_() {
  stack.push(true);
}
