#include "vm.h"

#include <sh/ranges.h>

#include "dzboundmethod.h"
#include "dzclass.h"
#include "dzinstance.h"
#include "dzlist.h"
#include "dznull.h"
#include "gc.h"
#include "opcode.h"

template<typename A, template<typename> typename Promote>
using unary_t = std::conditional_t<dz_primitive<A>, Promote<A>, A>;

template<typename A, typename B, template<typename, typename> typename Promote>
using binary_t = std::conditional_t<dz_primitive<A, B>, Promote<A, B>, A>;

Vm::Vm(Gc& gc)
  : gc(gc) {}

void Vm::interpret(DzFunction* main) {
  static_assert(int(Opcode::LastEnumValue) == 50);

  globals.resize(main->identifiers.size());
  frames.emplace(main->chunk().code.data(), 0, main);

  defineNativeFunctions();
  defineNativeMembersList();

  gc.vm = this;

  while (true) {
    opcode_pc = frames.top().pc;
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
      case Opcode::Invoke: invoke(); break;
      case Opcode::Jump: jump(); break;
      case Opcode::JumpFalse: jumpFalse(); break;
      case Opcode::JumpFalsePop: jumpFalsePop(); break;
      case Opcode::JumpTrue: jumpTrue(); break;
      case Opcode::Less: less(); break;
      case Opcode::LessEqual: lessEqual(); break;
      case Opcode::List: list<u8>(); break;
      case Opcode::ListExt: list<u16>(); break;
      case Opcode::Load: load<u8>(); break;
      case Opcode::LoadExt: load<u16>(); break;
      case Opcode::LoadGlobal: loadGlobal<u8>(); break;
      case Opcode::LoadGlobalExt: loadGlobal<u16>(); break;
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
      case Opcode::StoreGlobal: storeGlobal<u8>(); break;
      case Opcode::StoreGlobalExt: storeGlobal<u16>(); break;
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
  auto& frame = frames.top();
  const auto value = sh::cast<Integral>(*frame.pc);
  frame.pc += sizeof(Integral);
  return value;
}

template<template<typename> typename Promote, typename Callback>
void Vm::unary(std::string_view operation, Callback callback) {
  static_assert(int(DzValue::Type::LastEnumValue) == 4);

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

  const auto& a = stack.top();

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

  raise("bad operand type for '{}': '{}'", operation, a.kind());
}

template<template<typename, typename> typename Promote, typename Callback>
void Vm::binary(std::string_view operation, Callback callback) {
  static_assert(int(DzValue::Type::LastEnumValue) == 4);

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

  const auto& a = stack.peek(1);
  const auto& b = stack.peek(0);

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

  raise("bad operand types for '{}': '{}' and '{}'", operation, b.kind(), a.kind());
}

void Vm::expect(const DzValue& value, DzValue::Type type) {
  if (!value.is(type)) {
    raise("expected type '{}' but got '{}'", type, value.type);
  }
}

void Vm::expect(const DzValue& value, DzObject::Type type) {
  if (!value.is(DzValue::Type::Object)) {
    raise("expected type '{}' but got '{}'", type, value.type);
  } else if (!value.is(type)) {
    raise("expected type '{}' but got '{}'", type, value.o->type);
  }
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
  const auto argc = read<u8>();
  call(stack.peek(argc), argc);
}

void Vm::call(DzValue& callee, std::size_t argc) {
  if (callee.is(DzValue::Type::Object)) {
    switch (callee.o->type) {
      case DzObject::Type::Class: {
        const auto class_ = callee.as<DzClass>();
        callee = gc.construct<DzInstance>(class_);
        if (class_->init) {
          call(class_->init, argc);
        } else if (argc > 0) {
          raise("expected 0 argument(s) but got {}", argc);
        }
        return;
      }

      case DzObject::Type::Function: {
        call(callee.as<DzFunction>(), argc);
        return;
      }

      case DzObject::Type::BoundMethod: {
        const auto method = callee.as<DzBoundMethod>();
        callee = method->self;
        call(method->function, argc);
        return;
      }
    }
  }
  raise("'{}' is not callable", callee);
}

void Vm::call(DzFunction* function, std::size_t argc) {
  if (frames.size() == kMaximumRecursionDepth) {
    raise("maximum recursion depth exceeded");
  }
  if (function->arity && *function->arity != argc) {
    raise("expected {} argument(s) but got {}", *function->arity, argc);
  }
  if (function->isChunk()) {
    frames.emplace(function->chunk().code.data(), stack.size() - argc - 1, function);
  } else {
    stack.top() = function->native()(*this, argc);
  }
}

template<std::integral Integral>
void Vm::constant() {
  const auto index = read<Integral>();
  stack.push(frames.top().function->chunk().constants[index]);
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
  auto error = [this](const DzValue& prop, const DzValue& self) {
    raise("cannot get property '{}' of type '{}'", prop.repr(), self.kind());
  };

  const auto prop = stack.pop_value().as<DzString>();
  const auto self = stack.pop_value();

  if (self.is(DzValue::Type::Object)) {
    if (self.o->is(DzObject::Type::Instance)) {
      const auto instance = static_cast<DzInstance*>(self.o);
      if (const auto value = instance->get(prop)) {
        stack.push(*value);
      } else if (const auto function = instance->class_->get(prop)) {
        stack.push(gc.construct<DzBoundMethod>(instance, function));
      } else {
        stack.push(&null);
      }
    } else {
      const auto type = int(self.o->type);
      const auto iter = members[type].find(prop);
      if (iter != members[type].end()) {
        const auto& [identifier, function] = *iter;
        stack.push(gc.construct<DzBoundMethod>(self.o, function));
      } else {
        error(prop, self);
      }
    }
  } else {
    error(prop, self);
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

void Vm::invoke() {
  auto error = [this](const DzValue& prop, const DzValue& self) {
    raise("cannot get property '{}' of type '{}'", prop.repr(), self.kind());
  };

  const auto argc = read<u8>();
  const auto prop = stack.pop_value().as<DzString>();

  auto& self = stack.peek(argc);
  if (self.is(DzValue::Type::Object)) {
    if (self.o->is(DzObject::Type::Instance)) {
      const auto instance = static_cast<DzInstance*>(self.o);
      if (const auto value = instance->get(prop)) {
        self = *value;
      } else if (const auto function = instance->class_->get(prop)) {
        return call(function, argc);
      } else {
        self = &null;
      }
    } else {
      const auto type = int(self.o->type);
      const auto iter = members[type].find(prop);
      if (iter != members[type].end()) {
        const auto& [identifier, function] = *iter;
        return call(function, argc);
      } else {
        error(prop, self);
      }
    }
  } else {
    error(prop, self);
  }
  call(self, argc);
}

void Vm::jump() {
  frames.top().pc += read<s16>();
}

void Vm::jumpFalse() {
  const auto offset = read<s16>();
  if (!stack.top()) {
    frames.top().pc += offset;
  }
}

void Vm::jumpFalsePop() {
  const auto offset = read<s16>();
  if (!stack.pop_value()) {
    frames.top().pc += offset;
  }
}

void Vm::jumpTrue() {
  const auto offset = read<s16>();
  if (stack.top()) {
    frames.top().pc += offset;
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

template<std::integral Integral>
void Vm::list() {
  const auto size = read<Integral>();
  const auto list = gc.construct<DzList>();
  list->values.reserve(size);
  for (const auto& value : sh::range(stack.end() - size, stack.end())) {
    list->values.push_back(value);
  }
  stack.pop(size);
  stack.push(list);
}

template<std::integral Integral>
void Vm::load() {
  const auto index = read<Integral>();
  stack.push(stack[frames.top().sp + index]);
}

template<std::integral Integral>
void Vm::loadGlobal() {
  const auto index = read<Integral>();
  const auto& value = globals[index];
  if (value.isUndefined()) {
    const auto main = frames[0].function;
    for (const auto& identifier : main->identifiers) {
      if (identifier.second == index) {
        raise("undefined variable '{}'", identifier.first->data);
      }
    }
    raise("unknown undefined variable");
  }
  stack.push(value);
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

template<std::integral Integral>
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
  stack.pop(stack.size() - frames.top().sp);
  stack.push(result);
  frames.pop();
}

void Vm::set() {
  auto prop_v = stack.pop_value();
  auto inst_v = stack.pop_value();
  if (!inst_v.is(DzObject::Type::Instance)) {
    raise("cannot set property '{}' of type '{}'", prop_v.repr(), inst_v.kind());
  }

  auto prop = prop_v.as<DzString>();
  auto inst = inst_v.as<DzInstance>();
  inst->set(prop, stack.top());
}

template<std::integral Integral>
void Vm::store() {
  const auto index = read<Integral>();
  stack[frames.top().sp + index] = stack.top();
}

template<std::integral Integral>
void Vm::storeGlobal() {
  const auto index = read<Integral>();
  globals[index] = stack.top();
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
