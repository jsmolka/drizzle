#pragma once

#include "chunk.h"
#include "dzstring.h"

class DzFunction : public DzObject {
public:
  DzFunction();

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  DzString* identifier;
  std::size_t arity = 0;
  Chunk chunk;
};
