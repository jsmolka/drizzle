#pragma once

#include <optional>
#include <functional>
#include <variant>

#include "chunk.h"
#include "dzstring.h"

class DzFunction final : public DzObject {
public:
  using Arity  = std::optional<std::size_t>;
  using Native = std::function<DzValue(Vm&, std::size_t)>;

  DzFunction();
  DzFunction(DzString* identifier, const Arity& arity);
  DzFunction(DzString* identifier, const Arity& arity, const Native& native);

  auto repr() const -> std::string override final;
  void mark(Gc& gc) override final;

  auto isChunk() const -> bool;
  auto chunk() -> Chunk&;
  auto isNative() const -> bool;
  auto native() -> Native&;

  Arity arity;
  DzString* identifier;

private:
  std::variant<Chunk, Native> body;
};
