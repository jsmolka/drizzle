#pragma once

#include "chunk.h"
#include "dzobject.h"

class DzFunction : public DzObject {
public:
  DzFunction();

  operator bool() const;

  auto repr() const -> std::string;
  auto name() const -> std::string_view;

  std::string identifier;
  std::size_t definition = 0;
  std::size_t arity = 0;
  Chunk chunk;
};
