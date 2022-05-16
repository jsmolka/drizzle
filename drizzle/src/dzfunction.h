#pragma once

#include <functional>
#include <optional>
#include <variant>

#include "chunk.h"
#include "dzstring.h"
#include "map.h"

class Vm;

class DzFunction : public DzObject {
public:
  using Arity = std::optional<std::size_t>;
  using Native = std::function<DzValue(Vm&, std::size_t)>;

  DzFunction();
  DzFunction(DzString* identifier, Arity arity);
  DzFunction(DzString* identifier, Arity arity, const Native& native);

  operator bool() const;
  auto repr() const -> std::string;

  auto isChunk() const -> bool;
  auto chunk() -> Chunk&;
  auto isNative() const -> bool;
  auto native() -> Native&;

  Arity arity;
  DzString* identifier;
  Map<std::size_t> identifiers;

private:
  std::variant<Chunk, Native> body;
};
