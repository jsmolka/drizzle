#pragma once

#include "chunk.h"
#include "dzstring.h"

class DzFunction : public DzObject {
public:
  using Arity = std::optional<std::size_t>;

  DzFunction();
  DzFunction(DzString* identifier, Arity arity);

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  DzString* identifier;
  Arity arity;
  Chunk chunk;
};
