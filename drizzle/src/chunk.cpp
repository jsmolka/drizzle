#include "chunk.h"

#include <sh/ranges.h>

auto Chunk::size() const -> std::size_t {
  return code.size();
}

auto Chunk::line(std::size_t index) const -> std::size_t {
  for (const auto& [start, number] : sh::reversed(lines)) {
    if (start <= index) {
      return number;
    }
  }
  return -1;
}

void Chunk::write(u8 byte, std::size_t line) {
  if (lines.empty() || line > lines.back().number) {
    lines.emplace_back(size(), line);
  }
  code.push_back(byte);
}
