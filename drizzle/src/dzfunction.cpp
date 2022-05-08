#include "dzfunction.h"

#include "vm.h"

DzFunction::DzFunction()
  : DzFunction(nullptr, std::nullopt) {}

DzFunction::DzFunction(DzString* identifier, Arity arity)
  : DzObject(Type::Function), identifier(identifier), arity(arity), implementation(Chunk{}) {}

DzFunction::DzFunction(DzString* identifier, Arity arity, const Native& native)
  : DzObject(Type::Function), identifier(identifier), arity(arity), implementation(native) {}

DzFunction::operator bool() const {
  return true;
}

auto DzFunction::repr() const -> std::string {
  return fmt::format("<function {}>", identifier->repr());
}

auto DzFunction::name() const -> std::string_view {
  return "function";
}

auto DzFunction::isChunk() const -> bool {
  return std::holds_alternative<Chunk>(implementation);
}

auto DzFunction::chunk() -> Chunk& {
  return std::get<Chunk>(implementation);
}

auto DzFunction::isNative() const -> bool {
  return std::holds_alternative<Native>(implementation);
}

auto DzFunction::native() -> Native& {
  return std::get<Native>(implementation);
}

void DzFunction::call(Vm& vm, std::size_t argc) {
  if (vm.frames.size() == Vm::kMaximumRecursionDepth) {
    vm.raise("maximum recursion depth exceeded");
  }
  if (arity && *arity != argc) {
    vm.raise("expected {} argument(s) but got {}", *arity, argc);
  }
  if (isChunk()) {
    vm.frames.emplace(chunk().code.data(), vm.stack.size() - argc - 1, this);
  } else {
    vm.stack.top() = native()(vm, argc);
  }
}
