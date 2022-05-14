#pragma once

#include <functional>
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
  void operator()(Vm& vm, std::size_t argc);
  auto kind() const -> std::string_view;
  auto repr() const -> std::string;

  auto isChunk() const -> bool;
  auto isNative() const -> bool;
  auto chunk() -> Chunk&;
  auto native() -> Native&;

  Arity arity;
  DzString* identifier;
  Map<std::size_t> globals;

private:
  std::variant<Chunk, Native> body;
};
