#include "vm.h"

#include <sh/ranges.h>

#include "dzboundmethod.h"
#include "dzbytes.h"
#include "dzinstance.h"
#include "dzlist.h"
#include "dzmap.h"
#include "dznull.h"
#include "dzrange.h"
#include "gc.h"

Vm::Vm(Gc& gc)
  : gc(gc) {
  gc.vm = this;
}

void Vm::interpret(const Program& program) {
  static_assert(int(Opcode::LastEnumValue) == 121);

  this->program = program;

  globals.resize(program.globals.size());

  frame.pc = program.main->chunk().code.data();
  frame.sp = 0;
  frame.function = program.main;

  defineNatives();

  while (true) {
    opcode_pc = frame.pc;
    switch (static_cast<Opcode>(read<u8>())) {
      case Opcode::Add: add(); break;
      case Opcode::AddGeneric: addGeneric(); break;
      case Opcode::AddInt: addInt(); break;
      case Opcode::AddFloat: addFloat(); break;
      case Opcode::BitwiseAnd: bitwiseAnd(); break;
      case Opcode::BitwiseAndGeneric: bitwiseAndGeneric(); break;
      case Opcode::BitwiseAndInt: bitwiseAndInt(); break;
      case Opcode::BitwiseAsr: bitwiseAsr(); break;
      case Opcode::BitwiseAsrGeneric: bitwiseAsrGeneric(); break;
      case Opcode::BitwiseAsrInt: bitwiseAsrInt(); break;
      case Opcode::BitwiseComplement: bitwiseComplement(); break;
      case Opcode::BitwiseComplementGeneric: bitwiseComplementGeneric(); break;
      case Opcode::BitwiseComplementInt: bitwiseComplementInt(); break;
      case Opcode::BitwiseLsl: bitwiseLsl(); break;
      case Opcode::BitwiseLslGeneric: bitwiseLslGeneric(); break;
      case Opcode::BitwiseLslInt: bitwiseLslInt(); break;
      case Opcode::BitwiseLsr: bitwiseLsr(); break;
      case Opcode::BitwiseLsrGeneric: bitwiseLsrGeneric(); break;
      case Opcode::BitwiseLsrInt: bitwiseLsrInt(); break;
      case Opcode::BitwiseOr: bitwiseOr(); break;
      case Opcode::BitwiseOrGeneric: bitwiseOrGeneric(); break;
      case Opcode::BitwiseOrInt: bitwiseOrInt(); break;
      case Opcode::BitwiseXor: bitwiseXor(); break;
      case Opcode::BitwiseXorGeneric: bitwiseXorGeneric(); break;
      case Opcode::BitwiseXorInt: bitwiseXorInt(); break;
      case Opcode::Call: call(); break;
      case Opcode::Constant: constant<u8>(); break;
      case Opcode::ConstantExt: constant<u16>(); break;
      case Opcode::Divide: divide(); break;
      case Opcode::DivideGeneric: divideGeneric(); break;
      case Opcode::DivideInt: divideInt(); break;
      case Opcode::DivideFloat: divideFloat(); break;
      case Opcode::DivideInteger: divideInteger(); break;
      case Opcode::DivideIntegerGeneric: divideIntegerGeneric(); break;
      case Opcode::DivideIntegerInt: divideIntegerInt(); break;
      case Opcode::DivideIntegerFloat: divideIntegerFloat(); break;
      case Opcode::Equal: equal(); break;
      case Opcode::EqualGeneric: equalGeneric(); break;
      case Opcode::EqualInt: equalInt(); break;
      case Opcode::EqualFloat: equalFloat(); break;
      case Opcode::Exit: goto exit;
      case Opcode::False: false_(); break;
      case Opcode::Get: get(); break;
      case Opcode::Greater: greater(); break;
      case Opcode::GreaterGeneric: greaterGeneric(); break;
      case Opcode::GreaterInt: greaterInt(); break;
      case Opcode::GreaterFloat: greaterFloat(); break;
      case Opcode::GreaterEqual: greaterEqual(); break;
      case Opcode::GreaterEqualGeneric: greaterEqualGeneric(); break;
      case Opcode::GreaterEqualInt: greaterEqualInt(); break;
      case Opcode::GreaterEqualFloat: greaterEqualFloat(); break;
      case Opcode::In: in(); break;
      case Opcode::Invoke: invoke(); break;
      case Opcode::IterInit: iterInit(); break;
      case Opcode::IterAdvance: iterAdvance<u8>(); break;
      case Opcode::IterAdvanceExt: iterAdvance<u16>(); break;
      case Opcode::IterValue: iterValue<u8>(); break;
      case Opcode::IterValueExt: iterValue<u16>(); break;
      case Opcode::Jump: jump(); break;
      case Opcode::JumpFalse: jumpFalse(); break;
      case Opcode::JumpFalsePop: jumpFalsePop(); break;
      case Opcode::JumpTrue: jumpTrue(); break;
      case Opcode::JumpTruePop: jumpTruePop(); break;
      case Opcode::Less: less(); break;
      case Opcode::LessGeneric: lessGeneric(); break;
      case Opcode::LessInt: lessInt(); break;
      case Opcode::LessFloat: lessFloat(); break;
      case Opcode::LessEqual: lessEqual(); break;
      case Opcode::LessEqualGeneric: lessEqualGeneric(); break;
      case Opcode::LessEqualInt: lessEqualInt(); break;
      case Opcode::LessEqualFloat: lessEqualFloat(); break;
      case Opcode::List: list<u8>(); break;
      case Opcode::ListExt: list<u16>(); break;
      case Opcode::Load: load<u8>(); break;
      case Opcode::LoadExt: load<u16>(); break;
      case Opcode::LoadGlobal: loadGlobal<u8>(); break;
      case Opcode::LoadGlobalExt: loadGlobal<u16>(); break;
      case Opcode::Map: map<u8>(); break;
      case Opcode::MapExt: map<u16>(); break;
      case Opcode::Modulo: modulo(); break;
      case Opcode::ModuloGeneric: moduloGeneric(); break;
      case Opcode::ModuloInt: moduloInt(); break;
      case Opcode::ModuloFloat: moduloFloat(); break;
      case Opcode::Multiply: multiply(); break;
      case Opcode::MultiplyGeneric: multiplyGeneric(); break;
      case Opcode::MultiplyInt: multiplyInt(); break;
      case Opcode::MultiplyFloat: multiplyFloat(); break;
      case Opcode::Negate: negate(); break;
      case Opcode::NegateGeneric: negateGeneric(); break;
      case Opcode::NegateInt: negateInt(); break;
      case Opcode::NegateFloat: negateFloat(); break;
      case Opcode::Not: not_(); break;
      case Opcode::NotEqual: notEqual(); break;
      case Opcode::NotEqualGeneric: notEqualGeneric(); break;
      case Opcode::NotEqualInt: notEqualInt(); break;
      case Opcode::NotEqualFloat: notEqualFloat(); break;
      case Opcode::Null: null_(); break;
      case Opcode::Pop: pop(); break;
      case Opcode::PopMultiple: popMultiple<u8>(); break;
      case Opcode::PopMultipleExt: popMultiple<u16>(); break;
      case Opcode::Power: power(); break;
      case Opcode::PowerGeneric: powerGeneric(); break;
      case Opcode::PowerInt: powerInt(); break;
      case Opcode::PowerFloat: powerFloat(); break;
      case Opcode::Range: range(); break;
      case Opcode::Return: return_(); break;
      case Opcode::Set: set(); break;
      case Opcode::Store: store<u8>(); break;
      case Opcode::StoreExt: store<u16>(); break;
      case Opcode::StoreGlobal: storeGlobal<u8>(); break;
      case Opcode::StoreGlobalExt: storeGlobal<u16>(); break;
      case Opcode::SubscriptGet: subscriptGet(); break;
      case Opcode::SubscriptSet: subscriptSet(); break;
      case Opcode::Subtract: subtract(); break;
      case Opcode::SubtractGeneric: subtractGeneric(); break;
      case Opcode::SubtractInt: subtractInt(); break;
      case Opcode::SubtractFloat: subtractFloat(); break;
      case Opcode::SwitchCase: switchCase(); break;
      case Opcode::SwitchCaseGeneric: switchCaseGeneric(); break;
      case Opcode::SwitchCaseInt: switchCaseInt(); break;
      case Opcode::True: true_(); break;
      default:
        SH_UNREACHABLE;
        break;
    }
  }

exit:
  assert(stack.empty());
}

void Vm::expect(const DzValue& value, DzValue::Type type) {
  if (value.type != type) {
    raise("expected type '{}' but got '{}'", type, value.kind());
  }
}

void Vm::expect(const DzValue& value, DzObject::Type type) {
  if (!(value.isObject() && value->type == type)) {
    raise("expected type '{}' but got '{}'", type, value.kind());
  }
}

void Vm::expectArity(DzFunction::Arity expected, std::size_t got) {
  if (expected && *expected != got) {
    const char* format = *expected != 1
      ? "expected {} arguments but got {}"
      : "expected {} argument but got {}";
    raise(format, *expected, got);
  }
}

template<std::integral Integral>
auto Vm::read() -> Integral {
  const auto value = sh::cast<Integral>(*frame.pc);
  frame.pc += sizeof(Integral);
  return value;
}

void Vm::patch(Opcode opcode) {
  *(--frame.pc) = static_cast<u8>(opcode);
}

auto Vm::checkType(const DzValue& a, DzValue::Type type) -> bool {
  return a.type == type;
}

auto Vm::checkType(const DzValue& a, const DzValue& b, DzValue::Type type) -> bool {
  return (int(a.type) & int(b.type)) == int(type);
}

template<template<typename> typename Promote, typename Callback>
void Vm::unary(std::string_view operation, Callback callback) {
  auto& a = stack.top();
  try {
    a = DzValue::unary<Promote>(a, callback);
  } catch (const NotSupportedException&) {
    raise("unsupported operand type for '{}': '{}'", operation, a.kind());
  }
}

template<template<typename> typename Promote, typename Callback>
void Vm::binary(std::string_view operation, Callback callback) {
  auto [a, b] = peekBinary();
  try {
    a = DzValue::binary<Promote>(a, b, callback);
    stack.pop();
  } catch (const NotSupportedException&) {
    raise("unsupported operand types for '{}': '{}' and '{}'", operation, a.kind(), b.kind());
  }
}

auto Vm::peekBinary() -> std::tuple<DzValue&, DzValue&> {
  return std::forward_as_tuple(stack.peek(1), stack.peek(0));
}

auto Vm::forward(DzValue& iteree) -> DzValue {
  try {
    if (!iteree.isObject()) {
      throw NotSupportedException();
    }
    return iteree->makeIterator(*this);
  } catch (const NotSupportedException&) {
    raise("'{}' object is not reverse iterable", iteree.kind());
  }
}

auto Vm::reverse(DzValue& iteree) -> DzValue {
  try {
    if (!iteree.isObject()) {
      throw NotSupportedException();
    }
    return iteree->makeReverseIterator(*this);
  } catch (const NotSupportedException&) {
    raise("'{}' object is not iterable", iteree.kind());
  }
}

void Vm::add() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::AddGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::AddInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::AddFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::addGeneric() {
  binary("+", [this]<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a + b;
    } else if constexpr (dz_object<A, B>) {
      if (a->type == b->type) {
        switch (a->type) {
          case DzObject::Type::Bytes: {
            return gc.construct<DzBytes>(
              a->template as<DzBytes>()->data +
              b->template as<DzBytes>()->data
            );
          }
          case DzObject::Type::List: {
            return gc.construct<DzList>(
              a->template as<DzList>()->values +
              b->template as<DzList>()->values
            );
          }
          case DzObject::Type::String: {
            return gc.construct<DzString>(
              a->template as<DzString>()->data +
              b->template as<DzString>()->data
            );
          }
        }
      }
    }
    throw NotSupportedException();
  });
}

void Vm::addInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i += b.i;
    stack.pop();
  } else {
    patch(Opcode::AddGeneric);
  }
}

void Vm::addFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a.f += b.f;
    stack.pop();
  } else {
    patch(Opcode::AddGeneric);
  }
}

void Vm::bitwiseAnd() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::BitwiseAndGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::BitwiseAndInt;
        break;
    }
  }
  patch(opcode);
}

void Vm::bitwiseAndGeneric() {
  binary<promote_lax_t>("&", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a & b);
    } else if constexpr (dz_int<A, B>) {
      return a & b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseAndInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i &= b.i;
    stack.pop();
  } else {
    patch(Opcode::BitwiseAndGeneric);
  }
}

void Vm::bitwiseAsr() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::BitwiseAsrGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::BitwiseAsrInt;
        break;
    }
  }
  patch(opcode);
}

void Vm::bitwiseAsrGeneric() {
  binary(">>", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B>) {
      return a >> b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseAsrInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i >>= b.i;
    stack.pop();
  } else {
    patch(Opcode::BitwiseAsrGeneric);
  }
}

void Vm::bitwiseComplement() {
  auto& a = stack.top();
  auto opcode = Opcode::BitwiseComplementGeneric;
  switch (a.type) {
    case DzValue::Type::Int:
      opcode = Opcode::BitwiseComplementInt;
      break;
  }
  patch(opcode);
}

void Vm::bitwiseComplementGeneric() {
  unary("~", []<typename A>(const A& a) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A>) {
      return ~a;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseComplementInt() {
  auto& a = stack.top();
  if (checkType(a, DzValue::Type::Int)) {
    a.i = ~a.i;
  } else {
    patch(Opcode::BitwiseComplementGeneric);
  }
}

void Vm::bitwiseLsl() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::BitwiseLslGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::BitwiseLslInt;
        break;
    }
  }
  patch(opcode);
}

void Vm::bitwiseLslGeneric() {
  binary("<<", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B>) {
      return a << b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseLslInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i <<= b.i;
    stack.pop();
  } else {
    patch(Opcode::BitwiseLslGeneric);
  }
}

void Vm::bitwiseLsr() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::BitwiseLsrGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::BitwiseLsrInt;
        break;
    }
  }
  patch(opcode);
}

void Vm::bitwiseLsrGeneric() {
  binary(">>>", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B>) {
      return static_cast<dzint>(static_cast<std::make_unsigned_t<dzint>>(a) >> b);
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseLsrInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i = static_cast<dzint>(static_cast<std::make_unsigned_t<dzint>>(a.i) >> b.i);
    stack.pop();
  } else {
    patch(Opcode::BitwiseLsrGeneric);
  }
}

void Vm::bitwiseOr() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::BitwiseOrGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::BitwiseOrInt;
        break;
    }
  }
  patch(opcode);
}

void Vm::bitwiseOrGeneric() {
  binary<promote_lax_t>("|", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a | b);
    } else if constexpr (dz_int<A, B>) {
      return a | b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseOrInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i |= b.i;
    stack.pop();
  } else {
    patch(Opcode::BitwiseOrGeneric);
  }
}

void Vm::bitwiseXor() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::BitwiseXorGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::BitwiseXorInt;
        break;
    }
  }
  patch(opcode);
}

void Vm::bitwiseXorGeneric() {
  binary<promote_lax_t>("^", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a ^ b);
    } else if constexpr (dz_int<A, B>) {
      return a ^ b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseXorInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i ^= b.i;
    stack.pop();
  } else {
    patch(Opcode::BitwiseXorGeneric);
  }
}

void Vm::call() {
  const auto argc = read<u8>();
  call(stack.peek(argc), argc);
}

void Vm::call(DzValue& callee, std::size_t argc) {
  try {
    if (!callee.isObject()) {
      throw NotSupportedException();
    }
    switch (callee->type) {
      case DzObject::Type::Class: {
        const auto class_ = callee->as<DzClass>();
        callee = gc.construct<DzInstance>(class_);
        if (class_->init) {
          call(class_->init, argc);
        } else {
          expectArity(0, argc);
        }
        break;
      }
      case DzObject::Type::Function: {
        call(callee->as<DzFunction>(), argc);
        break;
      }
      case DzObject::Type::BoundMethod: {
        const auto method = callee->as<DzBoundMethod>();
        callee = method->self;
        call(method->function, argc);
        break;
      }
      default: {
        throw NotSupportedException();
      }
    }
  } catch (const NotSupportedException&) {
    raise("'{}' object is not callable", callee.kind());
  }
}

void Vm::call(DzFunction* function, std::size_t argc) {
  if (frames.size() == kMaximumRecursionDepth) {
    raise("maximum recursion depth exceeded");
  }
  expectArity(function->arity, argc);
  if (function->isChunk()) {
    frames.push(frame);
    frame.pc = function->chunk().code.data();
    frame.sp = stack.size() - argc - 1;
    frame.function = function;
  } else {
    stack.top() = function->native()(*this, argc);
  }
}

template<std::integral Integral>
void Vm::constant() {
  const auto index = read<Integral>();
  stack.push(frame.function->chunk().constants[index]);
}

void Vm::divide() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::DivideGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::DivideInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::DivideFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::divideGeneric() {
  binary("/", [this]<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      if (b == static_cast<B>(0)) {
        raise("division by zero");
      }
      return static_cast<dzfloat>(a) / static_cast<dzfloat>(b);
    }
    throw NotSupportedException();
  });
}

void Vm::divideInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    if (b.i == 0) {
      raise("division by zero");
    }
    a = static_cast<dzfloat>(a.i) / static_cast<dzfloat>(b.i);
    stack.pop();
  } else {
    patch(Opcode::DivideGeneric);
  }
}

void Vm::divideFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    if (b.f == 0.0) {
      raise("division by zero");
    }
    a.f /= b.f;
    stack.pop();
  } else {
    patch(Opcode::DivideGeneric);
  }
}

void Vm::divideInteger() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::DivideIntegerGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::DivideIntegerInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::DivideIntegerFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::divideIntegerGeneric() {
  binary("//", [this]<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
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
    throw NotSupportedException();
  });
}

void Vm::divideIntegerInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    if (b.i == 0) {
      raise("integer division by zero");
    }
    a.i /= b.i;
    stack.pop();
  } else {
    patch(Opcode::DivideGeneric);
  }
}

void Vm::divideIntegerFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    if (b.f == 0.0) {
      raise("integer division by zero");
    }
    a.f = std::floor(a.f / b.f);
    stack.pop();
  } else {
    patch(Opcode::DivideGeneric);
  }
}

void Vm::equal() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::EqualGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::EqualInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::EqualFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::equalGeneric() {
  const auto other = stack.pop_value();
  stack.top() = stack.top() == other;
}

void Vm::equalInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a = a.i == b.i;
    stack.pop();
  } else {
    patch(Opcode::EqualGeneric);
  }
}

void Vm::equalFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a = a.f == b.f;
    stack.pop();
  } else {
    patch(Opcode::EqualGeneric);
  }
}

void Vm::false_() {
  stack.emplace(false);
}

void Vm::get() {
  auto& prop = stack.peek(0);
  auto& self = stack.peek(1);
  try {
    if (!self.isObject()) {
      throw NotSupportedException();
    }
    self = self->getProp(*this, prop, true);
    stack.pop();
  } catch (const NotFoundException&) {
    raise("'{}' object has no property '{}'", self.kind(), prop->as<DzString>()->data);
  } catch (const NotSupportedException&) {
    raise("'{}' object has no properties", self.kind());
  }
}

void Vm::greater() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::GreaterGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::GreaterInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::GreaterFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::greaterGeneric() {
  binary(">", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a > b;
    }
    throw NotSupportedException();
  });
}

void Vm::greaterInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a = a.i > b.i;
    stack.pop();
  } else {
    patch(Opcode::GreaterGeneric);
  }
}

void Vm::greaterFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a = a.f > b.f;
    stack.pop();
  } else {
    patch(Opcode::GreaterGeneric);
  }
}

void Vm::greaterEqual() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::GreaterEqualGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::GreaterEqualInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::GreaterEqualFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::greaterEqualGeneric() {
  binary(">=", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a >= b;
    }
    throw NotSupportedException();
  });
}

void Vm::greaterEqualInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a = a.i >= b.i;
    stack.pop();
  } else {
    patch(Opcode::GreaterEqualGeneric);
  }
}

void Vm::greaterEqualFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a = a.f >= b.f;
    stack.pop();
  } else {
    patch(Opcode::GreaterEqualGeneric);
  }
}

void Vm::in() {
  auto& expr = stack.peek(0);
  auto& self = stack.peek(1);
  try {
    if (!self.isObject()) {
      throw NotSupportedException();
    }
    self = self->in(*this, expr);
    stack.pop();
  } catch (const NotSupportedException&) {
    raise("'{}' object does not support 'in'", self.kind());
  }
}

void Vm::invoke() {
  auto  argc = read<u8>();
  auto  prop = stack.pop_value();
  auto& self = stack.peek(argc);
  try {
    if (!self.isObject()) {
      throw NotSupportedException();
    }
    auto callee = self->getProp(*this, prop, false);
    call(callee, argc);
  } catch (const NotFoundException&) {
    raise("'{}' object has no property '{}'", self.kind(), prop->as<DzString>()->data);
  } catch (const NotSupportedException&) {
    raise("'{}' object has no properties", self.kind());
  }
}

void Vm::iterInit() {
  stack.top() = forward(stack.top());
}

template<std::integral Integral>
void Vm::iterAdvance() {
  const auto index = read<Integral>();
  stack[frame.sp + index]->template as<DzIterator>()->advance();
}

template<std::integral Integral>
void Vm::iterValue() {
  const auto index = read<Integral>();
  stack.push(stack[frame.sp + index]->template as<DzIterator>()->value(*this));
}

void Vm::jump() {
  frame.pc += read<s16>();
}

void Vm::jumpFalse() {
  frame.pc += stack.top() ? 2 : read<s16>();
}

void Vm::jumpFalsePop() {
  frame.pc += stack.pop_value() ? 2 : read<s16>();
}

void Vm::jumpTrue() {
  frame.pc += stack.top() ? read<s16>() : 2;
}

void Vm::jumpTruePop() {
  frame.pc += stack.pop_value() ? read<s16>() : 2;
}

void Vm::less() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::LessGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::LessInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::LessFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::lessGeneric() {
  binary("<", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a < b;
    }
    throw NotSupportedException();
  });
}

void Vm::lessInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a = a.i < b.i;
    stack.pop();
  } else {
    patch(Opcode::LessGeneric);
  }
}

void Vm::lessFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a = a.f < b.f;
    stack.pop();
  } else {
    patch(Opcode::LessGeneric);
  }
}

void Vm::lessEqual() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::LessEqualGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::LessEqualInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::LessEqualFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::lessEqualGeneric() {
  binary("<=", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a <= b;
    }
    throw NotSupportedException();
  });
}

void Vm::lessEqualInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a = a.i <= b.i;
    stack.pop();
  } else {
    patch(Opcode::LessEqualGeneric);
  }
}

void Vm::lessEqualFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a = a.f <= b.f;
    stack.pop();
  } else {
    patch(Opcode::LessEqualGeneric);
  }
}

template<std::integral Integral>
void Vm::list() {
  auto size = read<Integral>();
  auto list = gc.construct<DzList>();
  list->values.reserve(size);
  for (const auto& value : sh::range(stack.end() - size, stack.end())) {
    list->values.push_back(value);
  }
  stack.pop(size);
  stack.emplace(list);
}

template<std::integral Integral>
void Vm::load() {
  const auto index = read<Integral>();
  stack.push(stack[frame.sp + index]);
}

template<std::integral Integral>
void Vm::loadGlobal() {
  const auto  index = read<Integral>();
  const auto& value = globals[index];
  if (value.isUndefined()) {
    if (index < program.globals.size()) {
      raise("undefined variable '{}'", program.globals[index]);
    } else {
      raise("unknown undefined variable");
    }
  }
  stack.push(value);
}

template<std::integral Integral>
void Vm::map() {
  auto size = read<Integral>();
  auto map  = gc.construct<DzMap>();
  map->values.reserve(size);
  while (size--) {
    auto v = stack.pop_value();
    auto k = stack.pop_value();
    map->values.set(k, v);
  }
  stack.emplace(map);
}

void Vm::modulo() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::ModuloGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::ModuloInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::ModuloFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::moduloGeneric() {
  binary("%", [this]<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
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
    throw NotSupportedException();
  });
}

void Vm::moduloInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    if (b.i == 0) {
      raise("modulo by zero");
    }
    a.i %= b.i;
    stack.pop();
  } else {
    patch(Opcode::ModuloGeneric);
  }
}

void Vm::moduloFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    if (b.f == 0.0) {
      raise("modulo by zero");
    }
    a.f = std::fmod(a.f, b.f);
    stack.pop();
  } else {
    patch(Opcode::ModuloGeneric);
  }
}

void Vm::multiply() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::MultiplyGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::MultiplyInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::MultiplyFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::multiplyGeneric() {
  binary("*", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a * b;
    }
    throw NotSupportedException();
  });
}

void Vm::multiplyInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i *= b.i;
    stack.pop();
  } else {
    patch(Opcode::MultiplyGeneric);
  }
}

void Vm::multiplyFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a.f *= b.f;
    stack.pop();
  } else {
    patch(Opcode::MultiplyGeneric);
  }
}

void Vm::negate() {
  auto& a = stack.top();
  auto opcode = Opcode::NegateGeneric;
  switch (a.type) {
    case DzValue::Type::Int:
      opcode = Opcode::NegateInt;
      break;
    case DzValue::Type::Float:
      opcode = Opcode::NegateFloat;
      break;
  }
  patch(opcode);
}

void Vm::negateGeneric() {
  unary("-", []<typename A>(const A& a) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A> || dz_float<A>) {
      return -a;
    }
    throw NotSupportedException();
  });
}

void Vm::negateInt() {
  auto& a = stack.top();
  if (checkType(a, DzValue::Type::Int)) {
    a = -a.i;
  } else {
    patch(Opcode::NegateGeneric);
  }
}

void Vm::negateFloat() {
  auto& a = stack.top();
  if (checkType(a, DzValue::Type::Float)) {
    a = -a.f;
  } else {
    patch(Opcode::NegateGeneric);
  }
}

void Vm::not_() {
  stack.top() = !stack.top();
}

void Vm::notEqual() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::NotEqualGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::NotEqualInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::NotEqualFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::notEqualGeneric() {
  const auto other = stack.pop_value();
  stack.top() = stack.top() != other;
}

void Vm::notEqualInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a = a.i != b.i;
    stack.pop();
  } else {
    patch(Opcode::NotEqualGeneric);
  }
}

void Vm::notEqualFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a = a.f != b.f;
    stack.pop();
  } else {
    patch(Opcode::NotEqualGeneric);
  }
}

void Vm::null_() {
  stack.emplace(&null);
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
  auto [a, b] = peekBinary();
  auto opcode = Opcode::PowerGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::PowerInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::PowerFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::powerGeneric() {
  binary("**", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return std::pow(a, b);
    }
    throw NotSupportedException();
  });
}

void Vm::powerInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i = std::pow(a.i, b.i);
    stack.pop();
  } else {
    patch(Opcode::PowerGeneric);
  }
}

void Vm::powerFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a.f = std::pow(a.f, b.f);
    stack.pop();
  } else {
    patch(Opcode::PowerGeneric);
  }
}

void Vm::range() {
  expect(stack.peek(0), DzValue::Type::Int);
  expect(stack.peek(1), DzValue::Type::Int);
  const auto stop  = stack.pop_value().i;
  const auto start = stack.top().i;
  stack.top() = gc.construct<DzRange>(start, stop, 1);
}

void Vm::return_() {
  const auto ret = stack.pop_value();
  stack.pop(stack.size() - frame.sp);
  stack.push(ret);
  frame = frames.pop_value();
}

void Vm::set() {
  auto prop = stack.pop_value();
  auto self = stack.pop_value();
  try {
    if (!self.isObject()) {
      throw NotSupportedException();
    }
    self->setProp(*this, prop, stack.top());
  } catch (const NotSupportedException&) {
    raise("'{}' object has no properties", self.kind());
  }
}

template<std::integral Integral>
void Vm::store() {
  const auto index = read<Integral>();
  stack[frame.sp + index] = stack.top();
}

template<std::integral Integral>
void Vm::storeGlobal() {
  const auto index = read<Integral>();
  globals[index] = stack.top();
}

void Vm::subscriptGet() {
  auto& expr = stack.peek(0);
  auto& self = stack.peek(1);
  try {
    if (!self.isObject()) {
      throw NotSupportedException();
    }
    self = self->getExpr(*this, expr);
    stack.pop();
  } catch (const NotSupportedException&) {
    raise("'{}' object is not subscriptable", self.kind());
  }
}

void Vm::subscriptSet() {
  auto expr = stack.pop_value();
  auto self = stack.pop_value();
  try {
    if (!self.isObject()) {
      throw NotSupportedException();
    }
    self->setExpr(*this, expr, stack.top());
  } catch (const NotSupportedException&) {
    raise("'{}' object is not subscriptable", self.kind());
  }
}

void Vm::subtract() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::SubtractGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::SubtractInt;
        break;
      case DzValue::Type::Float:
        opcode = Opcode::SubtractFloat;
        break;
    }
  }
  patch(opcode);
}

void Vm::subtractGeneric() {
  binary("-", []<typename A, typename B>(const A& a, const B& b) SH_INLINE_LAMBDA -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a - b;
    }
    throw NotSupportedException();
  });
}

void Vm::subtractInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    a.i -= b.i;
    stack.pop();
  } else {
    patch(Opcode::SubtractGeneric);
  }
}

void Vm::subtractFloat() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Float)) {
    a.f -= b.f;
    stack.pop();
  } else {
    patch(Opcode::SubtractGeneric);
  }
}

void Vm::switchCase() {
  auto [a, b] = peekBinary();
  auto opcode = Opcode::SwitchCaseGeneric;
  if (a.type == b.type) {
    switch (a.type) {
      case DzValue::Type::Int:
        opcode = Opcode::SwitchCaseInt;
        break;
    }
  }
  patch(opcode);
}

void Vm::switchCaseGeneric() {
  auto [a, b] = peekBinary();
  if (a == b) {
    stack.pop(2);
    frame.pc += read<s16>();
  } else {
    stack.pop();
    frame.pc += 2;
  }
}

void Vm::switchCaseInt() {
  auto [a, b] = peekBinary();
  if (checkType(a, b, DzValue::Type::Int)) {
    if (a.i == b.i) {
      stack.pop(2);
      frame.pc += read<s16>();
    } else {
      stack.pop();
      frame.pc += 2;
    }
  } else {
    patch(Opcode::SwitchCaseGeneric);
  }
}

void Vm::true_() {
  stack.emplace(true);
}
