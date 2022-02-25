#include "utils.h"

namespace tests_vm_block {

inline suite _ = [] {
  "vm_block"_test = [] {
    constexpr const char* kSources[] = {
R"(
block x:
  while true:
    while true:
      break x
)",
R"(
var y = 0
block x:
  while true:
    while true:
      y = y + 1
      if y == 5:
        break x
assert(y == 5)
)",
R"(
var y = 0
block x:
  break x
  y = 1
assert(y == 0)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_block
