#include "dzfunction.h"

#include <sh/utility.h>

DzFunction::DzFunction()
  : DzFunction(nullptr, Arity::equal(0)) {}

DzFunction::DzFunction(DzString* identifier, const Arity& arity)
  : DzObject(Type::Function), identifier(identifier), arity(arity), body(Chunk{}) {}

DzFunction::DzFunction(DzString* identifier, const Arity& arity, const Native& native)
  : DzObject(Type::Function), identifier(identifier), arity(arity), body(native) {}

auto DzFunction::repr() const -> std::string {
  return fmt::format("<function {} at 0x{:016X}>", identifier->data, sh::cast<std::size_t>(this));
}

auto DzFunction::isChunk() const -> bool {
  return std::holds_alternative<Chunk>(body);
}

auto DzFunction::chunk() -> Chunk& {
  return std::get<Chunk>(body);
}

auto DzFunction::isNative() const -> bool {
  return std::holds_alternative<Native>(body);
}

auto DzFunction::native() -> Native& {
  return std::get<Native>(body);
}
