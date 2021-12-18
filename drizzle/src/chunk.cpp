#include "chunk.h"

#include <sh/ranges.h>

void Chunk::write(u8 byte, std::size_t line) {
  if (lines.empty() || line > lines.back().number) {
    lines.push_back({code.size(), line});
  }
  code.push_back(byte);
}

auto Chunk::label() const -> std::size_t {
  return code.size();
}

auto Chunk::line(std::size_t index) const -> std::size_t {
  for (const auto& line : sh::reversed(lines)) {
    if (line.index <= index) {
      return line.number;
    }
  }
  return -1;
}
