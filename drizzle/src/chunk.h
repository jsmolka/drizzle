#pragma once

#include <vector>

#include "dzvalue.h"

class Chunk {
public:
  auto size() const -> std::size_t;
  auto line(u8* pc) const -> std::size_t;
  void write(u8 byte, std::size_t line);

  std::vector<u8> code;
  std::vector<DzValue> constants;

private:
  struct Line {
    std::size_t number;
    std::size_t offset;
  };
  std::vector<Line> lines;
};
