#include "chunk.h"

#include <sh/ranges.h>
#include <sh/utility.h>

auto Chunk::size() const -> std::size_t {
  return code.size();
}

auto Chunk::line(std::size_t index) const -> std::size_t {
  for (const auto& [number, offset] : sh::reversed(lines)) {
    if (offset <= index) {
      return number;
    }
  }
  SH_UNREACHABLE;
  return 0;
}

void Chunk::write(u8 byte, std::size_t line) {
  if (lines.empty() || line > lines.back().number) {
    lines.emplace_back(line, size());
  }
  code.push_back(byte);
}
