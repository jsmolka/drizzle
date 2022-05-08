#pragma once

#include <functional>
#include <variant>

#include "chunk.h"
#include "dzstring.h"

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
  auto name() const -> std::string_view;
  auto isChunk() const -> bool;
  auto chunk() -> Chunk&;
  auto isNative() const -> bool;
  auto native() -> Native&;

  void call(Vm& vm, std::size_t argc);

  DzString* identifier;
  Arity arity;
  std::variant<Chunk, Native> implementation;
};
