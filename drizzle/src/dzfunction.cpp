#include "dzfunction.h"

#include "gc.h"

DzFunction::DzFunction()
  : DzFunction(nullptr, std::nullopt) {}

DzFunction::DzFunction(DzString* identifier, const Arity& arity)
  : DzObject(Type::Function), identifier(identifier), arity(arity), body(Chunk{}) {}

DzFunction::DzFunction(DzString* identifier, const Arity& arity, const Native& native)
  : DzObject(Type::Function), identifier(identifier), arity(arity), body(native) {}

auto DzFunction::repr() const -> std::string {
  return fmt::format("<function {} at 0x{:016X}>", identifier->data, sh::cast<std::size_t>(this));
}

void DzFunction::mark(Gc& gc) {
  DzObject::mark(gc);
  gc.mark(identifier);
  if (isChunk()) {
    for (const auto& constant : chunk().constants) {
      gc.mark(constant);
    }
  }
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
