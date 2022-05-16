#include "dzfunction.h"

DzFunction::DzFunction()
  : DzFunction(nullptr, std::nullopt) {}

DzFunction::DzFunction(DzString* identifier, Arity arity)
  : DzObject(Type::Function), identifier(identifier), arity(arity), body(Chunk{}) {}

DzFunction::DzFunction(DzString* identifier, Arity arity, const Native& native)
  : DzObject(Type::Function), identifier(identifier), arity(arity), body(native) {}

DzFunction::operator bool() const {
  return true;
}

auto DzFunction::repr() const -> std::string {
  return fmt::format("<function {}>", identifier->repr());
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
