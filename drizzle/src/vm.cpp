#include "vm.h"

#include "dzstring.h"
#include "errors.h"
#include "format.h"
#include "opcode.h"

Vm::Vm(StringPool& pool)
  : pool(pool) {}

void Vm::interpret(const Chunk& chunk) {
  this->chunk = &chunk;
  this->pc = chunk.code.data();

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
      case Opcode::Return: if (return_()) { return; } break;
      case Opcode::StoreVariable: storeVariable<u8>(); break;
      case Opcode::StoreVariableExt: storeVariable<u16>(); break;
      case Opcode::Subtract: subtract(); break;
      case Opcode::True: pushTrue(); break;
      default:
        SH_UNREACHABLE;
        break;
    }
  }
}

template<typename Integral>
Integral Vm::read() {
  static_assert(std::is_integral_v<Integral>);

  const auto value = sh::cast<Integral>(*pc);
  pc += sizeof(Integral);
  return value;
}

template<typename Error, typename... Args>
void Vm::raise(std::string_view message, Args&&... args) {
  static_assert(std::is_base_of_v<RuntimeError, Error>);

  const auto index = pc - chunk->code.data();
  const auto line = chunk->line(index);

  throw Error(line, message, std::forward<Args>(args)...);
}

template<template<typename T> typename Promote, typename Handler>
void Vm::unary(std::string_view operation, Handler handler) {
  static_assert(int(DzValue::Type::LastEnumValue) == 5);

  auto& value = stack.top();

  #define DZ_EVAL(a)                                    \
  {                                                     \
    using A = decltype(a);                              \
    if constexpr (dz_primitive<A>) {                    \
      if (handler(value, static_cast<Promote<A>>(a))) { \
        return;                                         \
      } else {                                          \
        break;                                          \
      }                                                 \
    } else {                                            \
      if (handler(value, a)) {                          \
        return;                                         \
      } else {                                          \
        break;                                          \
      }                                                 \
    }                                                   \
    break;                                              \
  }

  switch (value.type) {
    case DzValue::Type::Bool:   DZ_EVAL(value.b);
    case DzValue::Type::Int:    DZ_EVAL(value.i);
    case DzValue::Type::Float:  DZ_EVAL(value.f);
    case DzValue::Type::Null:   DZ_EVAL(DzNull{});
    case DzValue::Type::Object: DZ_EVAL(value.o);
    default:
      SH_UNREACHABLE;
      break;
  }

  #undef DZ_EVAL

  raise<RuntimeError>("bad operand type for '{}': '{}'", operation, value.typeName());
}

template<template<typename T, typename U> typename Promote, typename Handler>
void Vm::binary(std::string_view operation, Handler handler) {
  static_assert(int(DzValue::Type::LastEnumValue) == 5);

  auto rhs = stack.pop_value();
  auto& lhs = stack.top();

  #define DZ_EVAL(a, b)                                                                 \
  {                                                                                     \
    using A = decltype(a);                                                              \
    using B = decltype(b);                                                              \
    if constexpr (dz_primitive<A, B>) {                                                 \
      if (handler(lhs, static_cast<Promote<A, B>>(a), static_cast<Promote<A, B>>(b))) { \
        return;                                                                         \
      } else {                                                                          \
        break;                                                                          \
      }                                                                                 \
    } else {                                                                            \
      if (handler(lhs, a, b)) {                                                         \
        return;                                                                         \
      } else {                                                                          \
        break;                                                                          \
      }                                                                                 \
    }                                                                                   \
    break;                                                                              \
  }

  #define DZ_HASH(a, b) (int(DzValue::Type::LastEnumValue) * int(a) + int(b))

  switch (DZ_HASH(lhs.type, rhs.type)) {
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
      SH_UNREACHABLE;
      break;
  }

  #undef DZ_HASH
  #undef DZ_EVAL

  raise<RuntimeError>("bad operand types for '{}': '{}' and '{}'", operation, lhs.typeName(), rhs.typeName());
}

void Vm::add() {
  binary("+", [this]<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      dst = a + b;
      return true;
    } else if constexpr (dz_object<A, B>) {
      if (a->type == DzObject::Type::String && b->type == DzObject::Type::String) {
        auto aa = static_cast<DzString*>(a);
        auto bb = static_cast<DzString*>(b);
        dst = pool.make(aa->data + bb->data);
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

template<typename Integral>
void Vm::constant() {
  const auto index = read<Integral>();
  stack.push(chunk->constants[index]);
}

void Vm::divide() {
  binary("/", [this]<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise<RuntimeError>("division by zero");
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
        raise<RuntimeError>("integer division by zero");
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
    } else if constexpr (dz_object<A, B>) {
      dst = *a == *b;
    } else {
      dst = dz_null<A, B>;
    }
    return true;
  });
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
  pc += read<s16>();
}

void Vm::jumpFalse() {
  const auto offset = read<s16>();
  if (!stack.top()) {
    pc += offset;
  }
}

void Vm::jumpFalsePop() {
  const auto offset = read<s16>();
  if (!stack.pop_value()) {
    pc += offset;
  }
}

void Vm::jumpTrue() {
  const auto offset = read<s16>();
  if (stack.top()) {
    pc += offset;
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
void Vm::loadVariable() {
  const auto index = read<Integral>();
  stack.push(stack[index]);
}

void Vm::modulo() {
  binary("%", [this]<typename A, typename B>(DzValue& dst, const A& a, const B& b) {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise<RuntimeError>("modulo by zero");
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
    } else if constexpr (dz_object<A, B>) {
      dst = *a != *b;
    } else {
      dst = !dz_null<A, B>;
    }
    return true;
  });
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

void Vm::print() {
  fmt::print("{}\n", stack.pop_value());
}

void Vm::pushFalse() {
  stack.push(false);
}

void Vm::pushNull() {
  stack.push({});
}

void Vm::pushTrue() {
  stack.push(true);
}

bool Vm::return_() {
  return true;
}

template<typename Integral>
void Vm::storeVariable() {
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
