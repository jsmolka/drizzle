#pragma once

#include "dzvalue.h"

class Chunk {
 public:
  void write(u8 byte, std::size_t line);

  auto label() const -> std::size_t;
  auto line(std::size_t index) const -> std::size_t;

  DzValues constants;
  std::vector<u8> code;

 private:
  struct Line {
    std::size_t index;
    std::size_t number;
  };
  std::vector<Line> lines;
};
