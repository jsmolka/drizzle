#pragma

#include "utils.h"

namespace tests_vm_block {

inline suite _ = [] {
  "vm_block"_test = [] {
    constexpr const char* kSources[] = {
R"(
block label:
  while true:
    while true:
      break label
)",
R"(
var x = 0
block label:
  while true:
    while true:
      x = x + 1
      if x == 5:
        break label
assert(x == 5)
)",
R"(
var x = 0
block label:
  break label
  x = 1
assert(x == 0)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_block
