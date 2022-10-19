#include "vm.h"

#include <sh/ranges.h>

#include "dzboundmethod.h"
#include "dzbytes.h"
#include "dzclass.h"
#include "dzinstance.h"
#include "dzlist.h"
#include "dzmap.h"
#include "dznull.h"
#include "dzrange.h"
#include "gc.h"
#include "opcode.h"

Vm::Vm(Gc& gc)
  : gc(gc) {}

void Vm::interpret(const Program& program) {
  static_assert(int(Opcode::LastEnumValue) == 62);

  this->program = program;

  globals.resize(program.globals.size());
  frames.emplace(program.main->chunk().code.data(), 0, program.main);

  defineNatives();
  defineBytesMembers();
  defineListMembers();
  defineMapMembers();
  defineSdlWindowMembers();

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
      case Opcode::LessEqual: lessEqual(); break;
      case Opcode::List: list<u8>(); break;
      case Opcode::ListExt: list<u16>(); break;
      case Opcode::Load: load<u8>(); break;
      case Opcode::LoadExt: load<u16>(); break;
      case Opcode::LoadGlobal: loadGlobal<u8>(); break;
      case Opcode::LoadGlobalExt: loadGlobal<u16>(); break;
      case Opcode::Map: map<u8>(); break;
      case Opcode::MapExt: map<u16>(); break;
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
  auto& a = stack.top();
  try {
    a = DzValue::unary<Promote>(a, callback);
  } catch (const NotSupportedException&) {
    raise("unsupported operand type for '{}': '{}'", operation, a.kind());
  }
}

template<template<typename> typename Promote, typename Callback>
void Vm::binary(std::string_view operation, Callback callback) {
  auto& a = stack.peek(1);
  auto& b = stack.peek(0);
  try {
    a = a.binary<Promote>(a, b, callback);
    stack.pop();
  } catch (const NotSupportedException&) {
    raise("unsupported operand types for '{}': '{}' and '{}'", operation, a.kind(), b.kind());
  }
}

void Vm::expect(const DzValue& value, DzValue::Type type) {
  if (!value.is(type)) {
    raise("expected type '{}' but got '{}'", type, value.kind());
  }
}

void Vm::expect(const DzValue& value, DzObject::Type type) {
  if (!(value.isObject() && value->is(type))) {
    raise("expected type '{}' but got '{}'", type, value.kind());
  }
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
  binary("+", [this]<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a + b;
    } else if constexpr (dz_object<A, B>) {
      if (a->type == b->type) {
        switch (a->type) {
          case DzObject::Type::Bytes: {
            const auto bytes_a = a->template as<DzBytes>();
            const auto bytes_b = b->template as<DzBytes>();
            const auto bytes = gc.construct<DzBytes>();
            bytes->data.reserve(bytes_a->size() + bytes_b->size());
            for (const auto& data : {bytes_a->data, bytes_b->data}) {
              for (const auto& value : data) {
                bytes->data.push_back(value);
              }
            }
            return bytes;
          }
          case DzObject::Type::List: {
            const auto list_a = a->template as<DzList>();
            const auto list_b = b->template as<DzList>();
            const auto list = gc.construct<DzList>();
            list->values.reserve(list_a->size() + list_b->size());
            for (const auto& values : {list_a->values, list_b->values}) {
              for (const auto& value : values) {
                list->values.push_back(value);
              }
            }
            return list;
          }
          case DzObject::Type::String: {
            const auto string_a = a->template as<DzString>();
            const auto string_b = b->template as<DzString>();
            return gc.construct<DzString>(string_a->data + string_b->data);
          }
        }
      }
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseAnd() {
  binary<promote_lax_t>("&", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a & b);
    } else if constexpr (dz_int<A, B>) {
      return a & b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseAsr() {
  binary(">>", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B>) {
      return a >> b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseComplement() {
  unary("~", []<typename A>(const A& a) -> DzValue {
    if constexpr (dz_int<A>) {
      return ~a;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseLsl() {
  binary("<<", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B>) {
      return a << b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseLsr() {
  binary(">>>", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B>) {
      return static_cast<dzint>(static_cast<std::make_unsigned_t<dzint>>(a) >> b);
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseOr() {
  binary<promote_lax_t>("|", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a | b);
    } else if constexpr (dz_int<A, B>) {
      return a | b;
    }
    throw NotSupportedException();
  });
}

void Vm::bitwiseXor() {
  binary<promote_lax_t>("^", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_bool<A, B>) {
      return static_cast<dzbool>(a ^ b);
    } else if constexpr (dz_int<A, B>) {
      return a ^ b;
    }
    throw NotSupportedException();
  });
}

void Vm::call() {
  const auto argc = read<u8>();
  call(stack.peek(argc), argc);
}

void Vm::call(DzValue& callee, std::size_t argc) {
  auto error = [this](const DzValue& callee) {
    raise("'{}' object is not callable", callee.kind());
  };

  if (!callee.is(DzValue::Type::Object)) {
    error(callee);
  }

  switch (callee.o->type) {
    case DzObject::Type::Class: {
      const auto class_ = callee.o->as<DzClass>();
      callee = gc.construct<DzInstance>(class_);
      if (class_->init) {
        call(class_->init, argc);
      } else if (argc > 0) {
        raise(Arity::equal(0).message(argc));
      }
      break;
    }
    case DzObject::Type::Function: {
      call(callee.o->as<DzFunction>(), argc);
      break;
    }
    case DzObject::Type::BoundMethod: {
      const auto method = callee.o->as<DzBoundMethod>();
      callee = method->self;
      call(method->function, argc);
      break;
    }
    default: {
      error(callee);
      break;
    }
  }
}

void Vm::call(DzFunction* function, std::size_t argc) {
  if (frames.size() == kMaximumRecursionDepth) {
    raise("maximum recursion depth exceeded");
  }
  if (!function->arity.matches(argc)) {
    raise(function->arity.message(argc));
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
  binary("/", [this]<typename A, typename B>(const A& a, const B& b) -> DzValue {
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
  binary("//", [this]<typename A, typename B>(const A& a, const B& b) -> DzValue {
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

void Vm::equal() {
  const auto other = stack.pop_value();
  stack.top() = stack.top() == other;
}

void Vm::false_() {
  stack.push(false);
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
  binary(">", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a > b;
    }
    throw NotSupportedException();
  });
}

void Vm::greaterEqual() {
  binary(">=", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a >= b;
    }
    throw NotSupportedException();
  });
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
  stack[frames.top().sp + index]->template as<DzIterator>()->advance();
}

template<std::integral Integral>
void Vm::iterValue() {
  const auto index = read<Integral>();
  stack.push(stack[frames.top().sp + index]->template as<DzIterator>()->value(*this));
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

void Vm::jumpTruePop() {
  const auto offset = read<s16>();
  if (stack.pop_value()) {
    frames.top().pc += offset;
  }
}

void Vm::less() {
  binary("<", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a < b;
    }
    throw NotSupportedException();
  });
}

void Vm::lessEqual() {
  binary("<=", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a <= b;
    }
    throw NotSupportedException();
  });
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
    map->set(k, v);
  }
  stack.push(map);
}

void Vm::modulo() {
  binary("%", [this]<typename A, typename B>(const A& a, const B& b) -> DzValue {
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

void Vm::multiply() {
  binary("*", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a * b;
    }
    throw NotSupportedException();
  });
}

void Vm::negate() {
  unary("-", []<typename A>(const A& a) -> DzValue {
    if constexpr (dz_int<A> || dz_float<A>) {
      return -a;
    }
    throw NotSupportedException();
  });
}

void Vm::not_() {
  stack.top() = !stack.top();
}

void Vm::notEqual() {
  const auto other = stack.pop_value();
  stack.top() = stack.top() != other;
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
  binary("**", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return std::pow(a, b);
    }
    throw NotSupportedException();
  });
}

void Vm::range() {
  expect(stack.peek(0), DzValue::Type::Int);
  expect(stack.peek(1), DzValue::Type::Int);
  const auto stop  = stack.pop_value().i;
  const auto start = stack.top().i;
  stack.top() = gc.construct<DzRange>(start, stop, 1);
}

void Vm::return_() {
  const auto result = stack.pop_value();
  stack.pop(stack.size() - frames.top().sp);
  stack.push(result);
  frames.pop();
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
  stack[frames.top().sp + index] = stack.top();
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
  binary("-", []<typename A, typename B>(const A& a, const B& b) -> DzValue {
    if constexpr (dz_int<A, B> || dz_float<A, B>) {
      return a - b;
    }
    throw NotSupportedException();
  });
}

void Vm::true_() {
  stack.push(true);
}
