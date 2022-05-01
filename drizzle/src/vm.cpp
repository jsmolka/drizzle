#include "vm.h"

#include "dzbuiltin.h"
#include "dzclass.h"
#include "dzinstance.h"
#include "dznull.h"
#include "dzstring.h"
#include "gc.h"
#include "opcode.h"

template<typename A, template<typename> typename Promote>
using unary_t = std::conditional_t<dz_primitive<A>, Promote<A>, A>;

template<typename A, typename B, template<typename, typename> typename Promote>
using binary_t = std::conditional_t<dz_primitive<A, B>, Promote<A, B>, A>;

Vm::Vm(Gc& gc)
  : gc(gc) {
  gc.vm = this;
}

void Vm::interpret(DzFunction* function) {
  static_assert(int(Opcode::LastEnumValue) == 47);

  frame.pc = function->chunk.code.data();
  frame.sp = 0;
  frame.function = function;

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
      case Opcode::Exit: goto exit;
      case Opcode::False: false_(); break;
      case Opcode::Get: get(); break;
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
      case Opcode::Null: null_(); break;
      case Opcode::Pop: pop(); break;
      case Opcode::PopMultiple: popMultiple<u8>(); break;
      case Opcode::PopMultipleExt: popMultiple<u16>(); break;
      case Opcode::Power: power(); break;
      case Opcode::Return: return_(); break;
      case Opcode::Set: set(); break;
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

exit:
  assert(stack.empty());
}

template<std::integral Integral>
auto Vm::read() -> Integral {
  const auto value = sh::cast<Integral>(*frame.pc);
  frame.pc += sizeof(Integral);
  return value;
}

template<typename... Args>
void Vm::raise(std::string_view format, Args&&... args) {
  const auto line = frame.function->chunk.line(
    frame.pc -
    frame.function->chunk.code.data());
  throw RuntimeError(Location{.line = line}, format, std::forward<Args>(args)...);
}

template<template<typename> typename Promote, typename Callback>
void Vm::unary(std::string_view operation, Callback callback) {
  static_assert(int(DzValue::Type::LastEnumValue) == 4);

  const auto& a = stack.top();

  #define DZ_EVAL(a)                   \
  {                                    \
    using A  = decltype(a);            \
    using AP = unary_t<A, Promote>;    \
    if (auto res = callback(AP(a))) {  \
      stack.top() = *res;              \
      return;                          \
    } else {                           \
      break;                           \
    }                                  \
  }

  switch (a.type) {
    case DzValue::Type::Bool:   DZ_EVAL(a.b);
    case DzValue::Type::Int:    DZ_EVAL(a.i);
    case DzValue::Type::Float:  DZ_EVAL(a.f);
    case DzValue::Type::Object: DZ_EVAL(a.o);
    default:
      SH_UNREACHABLE;
      break;
  }

  #undef DZ_EVAL

  raise("bad operand type for '{}': '{}'", operation, a.name());
}

template<template<typename, typename> typename Promote, typename Callback>
void Vm::binary(std::string_view operation, Callback callback) {
  static_assert(int(DzValue::Type::LastEnumValue) == 4);

  const auto& a = stack.peek(1);
  const auto& b = stack.peek(0);

  #define DZ_HASH(a, b) int(DzValue::Type::LastEnumValue) * int(a) + int(b)
  #define DZ_EVAL(a, b)                       \
  {                                           \
    using A  = decltype(a);                   \
    using B  = decltype(b);                   \
    using AP = binary_t<A, B, Promote>;       \
    using BP = binary_t<B, A, Promote>;       \
    if (auto res = callback(AP(a), BP(b))) {  \
      stack.pop();                            \
      stack.top() = *res;                     \
      return;                                 \
    } else {                                  \
      break;                                  \
    }                                         \
  }

  switch (DZ_HASH(a.type, b.type)) {
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Bool  ): DZ_EVAL(a.b, b.b);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Int   ): DZ_EVAL(a.b, b.i);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Float ): DZ_EVAL(a.b, b.f);
    case DZ_HASH(DzValue::Type::Bool,   DzValue::Type::Object): DZ_EVAL(a.b, b.o);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Bool  ): DZ_EVAL(a.i, b.b);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Int   ): DZ_EVAL(a.i, b.i);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Float ): DZ_EVAL(a.i, b.f);
    case DZ_HASH(DzValue::Type::Int,    DzValue::Type::Object): DZ_EVAL(a.i, b.o);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Bool  ): DZ_EVAL(a.f, b.b);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Int   ): DZ_EVAL(a.f, b.i);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Float ): DZ_EVAL(a.f, b.f);
    case DZ_HASH(DzValue::Type::Float,  DzValue::Type::Object): DZ_EVAL(a.f, b.o);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Bool  ): DZ_EVAL(a.o, b.b);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Int   ): DZ_EVAL(a.o, b.i);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Float ): DZ_EVAL(a.o, b.f);
    case DZ_HASH(DzValue::Type::Object, DzValue::Type::Object): DZ_EVAL(a.o, b.o);
    default:
      SH_UNREACHABLE;
      break;
  }

  #undef DZ_EVAL
  #undef DZ_HASH

  raise("bad operand types for '{}': '{}' and '{}'", operation, b.name(), a.name());
}

void Vm::add() {
  binary("+", [this]<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a + b;
    } else if constexpr (dz_object<A, B>) {
      if (a->is(DzObject::Type::String) && b->is(DzObject::Type::String)) {
        auto str_a = static_cast<DzString*>(a);
        auto str_b = static_cast<DzString*>(b);
        return gc.construct<DzString>(str_a->data + str_b->data);
      }
    }
    return std::nullopt;
  });
}

void Vm::bitwiseAnd() {
  binary<promote_lax_t>("&", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a & b);
    } else if constexpr (dz_int<A, B>) {
      return a & b;
    }
    return std::nullopt;
  });
}

void Vm::bitwiseAsr() {
  binary(">>", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B>) {
      return a >> b;
    }
    return std::nullopt;
  });
}

void Vm::bitwiseComplement() {
  unary("~", []<typename A>(const A& a) -> std::optional<DzValue> {
    if constexpr (dz_int<A>) {
      return ~a;
    }
    return std::nullopt;
  });
}

void Vm::bitwiseLsl() {
  binary("<<", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B>) {
      return a << b;
    }
    return std::nullopt;
  });
}

void Vm::bitwiseLsr() {
  binary(">>>", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B>) {
      return static_cast<dzint>(static_cast<std::make_unsigned_t<dzint>>(a) >> b);
    }
    return std::nullopt;
  });
}

void Vm::bitwiseOr() {
  binary<promote_lax_t>("|", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a | b);
    } else if constexpr (dz_int<A, B>) {
      return a | b;
    }
    return std::nullopt;
  });
}

void Vm::bitwiseXor() {
  binary<promote_lax_t>("^", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a ^ b);
    } else if constexpr (dz_int<A, B>) {
      return a ^ b;
    }
    return std::nullopt;
  });
}

void Vm::call() {
  auto exec = [this](DzFunction* function, std::size_t argc) {
    if (function->arity != argc) {
      raise("expected {} argument(s) but got {}", function->arity, argc);
    }
    frames.push(frame);
    if (frames.size() == kMaximumRecursionDepth) {
      raise("maximum recursion depth exceeded");
    }
    frame.pc = function->chunk.code.data();
    frame.sp = stack.size() - argc - 1;
    frame.function = function;
  };

  const auto argc = read<u8>();
  auto& callee = stack.peek(argc);
  if (callee.type == DzValue::Type::Object) {
    switch (callee.o->type) {
      case DzObject::Type::BuiltIn: {
        const auto builtin = static_cast<DzBuiltIn*>(callee.o);
        if (builtin->arity && *builtin->arity != argc) {
          raise("expected {} argument(s) but got {}", *builtin->arity, argc);
        }
        stack.top() = builtin->callback(*this, argc);
        return;
      }

      case DzObject::Type::Class: {
        const auto class_ = static_cast<DzClass*>(callee.o);
        auto instance = gc.construct<DzInstance>(gc, class_);
        auto init = gc.construct<DzString>(std::string_view("init"));
        auto i = instance->fields.find(init);
        if (i != instance->fields.end()) {
          const auto function = static_cast<DzBoundMethod*>(i->second.o)->function;
          callee = static_cast<DzBoundMethod*>(i->second.o)->self;
          return exec(function, argc);
        } else if (argc > 0) {
          raise("expected {} argument(s) but got {}", 0, argc);
        }
        stack.peek(argc) = instance;
        return;
      }

      case DzObject::Type::BoundMethod: {
        const auto function = static_cast<DzBoundMethod*>(callee.o)->function;
        callee = static_cast<DzBoundMethod*>(callee.o)->self;
        return exec(function, argc);
      }

      case DzObject::Type::Function: {
        return exec(static_cast<DzFunction*>(callee.o), argc);
      }
    }
  }
  raise("'{}' is not callable", callee);
}

template<typename Integral>
void Vm::constant() {
  const auto index = read<Integral>();
  stack.push(frame.function->chunk.constants[index]);
}

void Vm::divide() {
  binary("/", [this]<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise("division by zero");
      }
      return static_cast<dzfloat>(a) / static_cast<dzfloat>(b);
    }
    return std::nullopt;
  });
}

void Vm::divideInt() {
  binary("//", [this]<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise("integer division by zero");
      }
      if constexpr (dz_int<A, B>) {
        return a / b;
      } else {
        return std::floor(a / b);
      }
    }
    return std::nullopt;
  });
}

void Vm::equal() {
  binary("==", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_primitive<A, B>) {
      return a == b;
    } else if constexpr (dz_object<A, B>) {
      return *a == *b;
    }
    return std::nullopt;
  });
}

void Vm::false_() {
  stack.push(false);
}

void Vm::get() {
  auto prop_v = stack.pop_value();
  auto self_v = stack.pop_value();

  assert(prop_v.is(DzObject::Type::String));
  if (!self_v.is(DzObject::Type::Instance)) {
    raise("cannot get property '{}' of type '{}'", prop_v.repr(), self_v.name());
  }

  auto prop = static_cast<DzString*>(prop_v.o);
  auto self = static_cast<DzInstance*>(self_v.o);

  auto pos = self->fields.find(prop);
  if (pos != self->fields.end()) {
    stack.push(pos->second);
  } else {
    stack.push(&null);
  }
}

void Vm::greater() {
  binary(">", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a > b;
    }
    return std::nullopt;
  });
}

void Vm::greaterEqual() {
  binary(">=", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a >= b;
    }
    return std::nullopt;
  });
}

void Vm::jump() {
  frame.pc += read<s16>();
}

void Vm::jumpFalse() {
  const auto offset = read<s16>();
  if (!stack.top()) {
    frame.pc += offset;
  }
}

void Vm::jumpFalsePop() {
  const auto offset = read<s16>();
  if (!stack.pop_value()) {
    frame.pc += offset;
  }
}

void Vm::jumpTrue() {
  const auto offset = read<s16>();
  if (stack.top()) {
    frame.pc += offset;
  }
}

void Vm::less() {
  binary("<", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a < b;
    }
    return std::nullopt;
  });
}

void Vm::lessEqual() {
  binary("<=", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a <= b;
    }
    return std::nullopt;
  });
}

template<typename Integral>
void Vm::load() {
  const auto index = read<Integral>();
  stack.push(stack[frame.sp + index]);
}

template<typename Integral>
void Vm::loadAbsolute() {
  const auto index = read<Integral>();
  stack.push(stack[index]);
}

void Vm::modulo() {
  binary("%", [this]<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise("modulo by zero");
      }
      if constexpr (dz_int<A, B>) {
        return a % b;
      } else {
        return std::fmod(a, b);
      }
    }
    return std::nullopt;
  });
}

void Vm::multiply() {
  binary("*", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a * b;
    }
    return std::nullopt;
  });
}

void Vm::negate() {
  unary("-", []<typename A>(const A& a) -> std::optional<DzValue> {
    if constexpr (dz_int<A> || dz_float<A>) {
      return -a;
    }
    return std::nullopt;
  });
}

void Vm::not_() {
  stack.top() = !stack.top();
}

void Vm::notEqual() {
  binary("!=", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_primitive<A, B>) {
      return a != b;
    } else if constexpr (dz_object<A, B>) {
      return *a != *b;
    }
    return std::nullopt;
  });
}

void Vm::null_() {
  stack.push(&null);
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
  binary("**", []<typename  A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return std::pow(a, b);
    }
    return std::nullopt;
  });
}

void Vm::return_() {
  const auto result = stack.pop_value();
  stack.pop(stack.size() - frame.sp);
  stack.push(result);
  frame = frames.pop_value();
}

void Vm::set() {
  auto prop_v = stack.pop_value();
  auto self_v = stack.pop_value();

  assert(prop_v.is(DzObject::Type::String));
  if (!self_v.is(DzObject::Type::Instance)) {
    raise("cannot set property '{}' of type '{}'", prop_v.repr(), self_v.name());
  }

  auto prop = static_cast<DzString*>(prop_v.o);
  auto self = static_cast<DzInstance*>(self_v.o);

  self->fields.insert_or_assign(prop, stack.top());
}

template<typename Integral>
void Vm::store() {
  const auto index = read<Integral>();
  stack[frame.sp + index] = stack.top();
}

template<typename Integral>
void Vm::storeAbsolute() {
  const auto index = read<Integral>();
  stack[index] = stack.top();
}

void Vm::subtract() {
  binary("-", []<typename A, typename B>(const A& a, const B& b) -> std::optional<DzValue> {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a - b;
    }
    return std::nullopt;
  });
}

void Vm::true_() {
  stack.push(true);
}
